#include "MainWindow.hpp"
#include "Filter.hpp"
#include "Port.hpp"
#include <thread>

#include <tins/tins.h>

MainWindow::MainWindow(SwSwitch& swSwitch, QWidget *parent)
    : QMainWindow(parent), swSwitch_{ swSwitch }
{
    ui.setupUi(this);

    // start button
    QPushButton* QPushButton_startButton = MainWindow::findChild<QPushButton*>("startButton");
    QObject::connect(QPushButton_startButton, SIGNAL(released()), this, SLOT(startButtonPressed()));

    // clear button
    QPushButton* QPushButton_closeButton = MainWindow::findChild<QPushButton*>("clearButton");
    QObject::connect(QPushButton_closeButton, SIGNAL(released()), this, SLOT(clearButtonPressed()));

    // setTimer btn
    QPushButton* QPushButton_setTimerButton = MainWindow::findChild<QPushButton*>("setTimerButton");
    QObject::connect(QPushButton_setTimerButton, SIGNAL(released()), this, SLOT(setTimerButtonPressed()));

    // setFilter btn
    QPushButton* QPushButton_setFilterButton = MainWindow::findChild<QPushButton*>("filterSetButton");
    QObject::connect(QPushButton_setFilterButton, SIGNAL(released()), this, SLOT(setFiltersButtonPressed()));

    // set timer line edit to accept only int values
    this->ui.setTimerLineEdit->setValidator(new QIntValidator(this));

    // delete filter button
    QPushButton* QPushButton_deleteFilterButton = MainWindow::findChild<QPushButton*>("filterDeleteButton");
    QObject::connect(QPushButton_deleteFilterButton, SIGNAL(released()), this, SLOT(deleteFilterButtonPressed()));

    // connect close button to stop a thread
    // QObject::connect(QPushButton_closeButton, SIGNAL(released()), &this->swSwitch_.displayQThread_, SLOT(stop()));

    // connect thread to a write PDU
    QObject::connect(&this->swSwitch_.displayQThread_, SIGNAL(finished()), this, SLOT(writePDU()));
}

void MainWindow::startButtonPressed()
{
    if (this->startButtonClicked_)
        return;
    this->startButtonClicked_ = true;
    QPushButton* button = (QPushButton*)sender();

    std::thread thread_port1(&Port::captureTraffic, &this->swSwitch_.port1_, &this->swSwitch_.port2_);
    std::thread thread_port2(&Port::captureTraffic, &this->swSwitch_.port2_, &this->swSwitch_.port1_);
    std::thread thread_port_buffer(&MainWindow::checkBuffer, this);
    std::thread thread_cam(&SwSwitch::updateCAM, &this->swSwitch_);
    //std::thread thread_loop(&SwSwitch::checkLoopbackProtocol, &this->swSwitch_);
	    
    thread_port1.detach();
    thread_port2.detach();
    thread_port_buffer.detach();
    thread_cam.detach();
    //thread_loop.detach();
}

void MainWindow::deleteFilterButtonPressed()
{
    QPushButton* button = (QPushButton*)sender();
    int i = this->ui.deleteFilterLineEdit->text().toInt();
    int j = this->ui.filterPortDeleteLineEdit->text().toInt();

    if (j < 1 || j > 2) return;
    if (j == 1 && i < this->swSwitch_.port1_.v_filters_.size())
    {
        this->swSwitch_.port1_.v_filters_.erase(this->swSwitch_.port1_.v_filters_.begin() + i);
    }
    if (j == 2 && i < this->swSwitch_.port2_.v_filters_.size())
    {
        this->swSwitch_.port2_.v_filters_.erase(this->swSwitch_.port2_.v_filters_.begin() + i);
    }
}

void MainWindow::clearButtonPressed()
{
    if (!this->startButtonClicked_) 
        return;
    QPushButton* button = (QPushButton*)sender();

    this->swSwitch_.port1_.clearIOStatistics();
    this->swSwitch_.port2_.clearIOStatistics();
    this->swSwitch_.clearCAM();

    this->writePDU();
}

void MainWindow::setTimerButtonPressed()
{
    QPushButton* button = (QPushButton*)sender();    
    int timer = this->ui.setTimerLineEdit->text().toInt();
    if (timer < 1)
        return;
    this->swSwitch_.initialSeconds_ = timer;
}


void MainWindow::setFiltersButtonPressed()
{
    QPushButton* button = (QPushButton*)sender();

    //// clear everything
    //this->swSwitch_.port1_.port_number_ = 0;
    //this->swSwitch_.port2_.port_number_ = 0;
    Filter filter{};
    // get port number
    int i_port_number = this->ui.filterSetPortLineEdit->text().toInt();

    // get another filter parameters
    std::string
        filter_direction = this->ui.filterIOLineEdit->text().toStdString(),
        filter_permit_deny = this->ui.filterPermitDenyLineEdit->text().toStdString();
    if (filter_permit_deny == "deny")
    {
        filter.b_filter_deny = true;
    }
    else
    {
        filter.b_filter_deny = false;
    }

    if (filter_direction == "out")
    {
        filter.b_filter_out = true;
    }
    else
    {
        filter.b_filter_out = false;
    }

    filter.filter_src_ip_add_ = this->ui.filterSetIPLineEdit->text().toStdString();
    filter.filter_src_mac_add_ = this->ui.filterSetMACLineEdit->text().toStdString();
    filter.filter_dst_ip_add_ = this->ui.filterSetIPLineEdit->text().toStdString();
    filter.filter_dst_mac_add_ = this->ui.filterSetMACLineEdit->text().toStdString();

    // find protocols
    std::string filter_protocol = this->ui.filterProtocolLineEdit->text().toStdString();
    if (filter_protocol.find("http") != std::string::npos)
    {
        filter.b_http_ = true;
    }
    else
    {
        filter.b_http_ = false;
    }

    if (filter_protocol.find("icmp") != std::string::npos)
    {
        filter.b_icmp_ = true;
    }
    else
    {
        filter.b_icmp_ = false;
    }

    if (filter_protocol.find("tcp") != std::string::npos)
    {
        filter.b_tcp_ = true;
    }
    else
    {
        filter.b_tcp_ = false;
    }

    if (filter_protocol.find("udp") != std::string::npos)
    {
        filter.b_udp_ = true;
    }
    else
    {
        filter.b_udp_ = false;
    }

    if (filter_protocol.find("ip") != std::string::npos)
    {
        filter.b_ip_ = true;
    }
    else
    {
        filter.b_ip_ = false;
    }

    if (filter_protocol.find("arp") != std::string::npos)
    {
        filter.b_arp_ = true;
    }
    else
    {
        filter.b_arp_ = false;
    }

    if (i_port_number == 1)
    {
        this->swSwitch_.port1_.v_filters_.push_back(filter);
    }
    else if (i_port_number == 2)
    {
        this->swSwitch_.port2_.v_filters_.push_back(filter);
    }

    qDebug()
        << "HTTP"  << ' ' << filter.b_http_  << '\n'
        << "ICMP"  << ' ' << filter.b_icmp_  << '\n'
        << "TCP"   << ' ' << filter.b_tcp_   << '\n'
        << "UDP"   << ' ' << filter.b_udp_   << '\n'
        << "IP"    << ' ' << filter.b_ip_    << '\n'
        << "ARP"   << ' ' << filter.b_arp_   << '\n';

    qDebug()
        << filter.b_filter_out << ' '
        << filter_protocol.c_str() << ' '
        << filter.filter_src_ip_add_.c_str() << ' '
        << filter.filter_src_mac_add_.c_str() << ' '
        << filter.b_filter_deny << '\n';
}

void MainWindow::writePDU()
{
    this->writeStatistics();
    this->writeCAM_Table();
}

void MainWindow::checkBuffer()
{
    while (1)
    {
        if (!this->swSwitch_.port1_.getBuffer().empty())
        {
            // std::lock_guard<std::mutex> lock(this->swSwitch_.port1_.mtx);
            //this->swSwitch_.port1_.wait_ = true;
            this->swSwitch_.checkCAM(this->swSwitch_.port1_, *this->swSwitch_.port1_.getBuffer()[0]);
            this->swSwitch_.port1_.getBuffer().pop_front(); 
            //this->swSwitch_.port1_.wait_ = false;
            //this->swSwitch_.displayQThread_.start();
            // this->swSwitch_.displayQThread_.quit();
        }

        if (!this->swSwitch_.port2_.getBuffer().empty())
        {
            // std::lock_guard<std::mutex> lock(this->swSwitch_.port2_.mtx);
            //this->swSwitch_.port2_.wait_ = true;
            this->swSwitch_.checkCAM(this->swSwitch_.port2_, *this->swSwitch_.port2_.getBuffer()[0]);
            this->swSwitch_.port2_.getBuffer().pop_front();
            //this->swSwitch_.port2_.wait_ = false;
            //this->swSwitch_.displayQThread_.start();
            // this->swSwitch_.displayQThread_.quit();
        }
    }
}

void MainWindow::writeStatistics()
{
	this->ui.statisticsDisplay->setText(QString::fromStdString(this->swSwitch_.port1_.getPortStatistics()));
	this->ui.statisticsDisplay->append(QString::fromStdString(this->swSwitch_.port2_.getPortStatistics()));

    this->ui.FilterTableDisplay->setText(QString::fromStdString(this->swSwitch_.port1_.getFilter()));
    this->ui.FilterTableDisplay->append(QString::fromStdString(this->swSwitch_.port2_.getFilter()));
}

std::string MainWindow::getCAM_Table()
{
    std::string strCamTable;
    // std::clock_t now = std::clock();
    for (auto it = this->swSwitch_.camTable_.begin(); it != this->swSwitch_.camTable_.end(); ++it)
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            // std::string clock_str = std::to_string(this->swSwitch_.initialSeconds_ - (std::clock() - it2->second) / CLOCKS_PER_SEC);
            strCamTable += it->first.to_string() + "\t" + it2->first + "\t" + std::to_string(it2->second) + "\n";
        }
    }
    return ("\tMAC\tPort\tTimer\n" + strCamTable);
}

void MainWindow::writeCAM_Table()
{
    this->ui.camTableDisplay->setText(QString::fromStdString(this->getCAM_Table()));
}