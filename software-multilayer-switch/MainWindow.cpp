#include "MainWindow.hpp"
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

    // set line edit to accept only int values
    this->ui.setTimerLineEdit->setValidator(new QIntValidator(this));

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
    std::thread thread_loop(&SwSwitch::checkLoopbackProtocol, &this->swSwitch_);
	    
    thread_port1.detach();
    thread_port2.detach();
    thread_port_buffer.detach();
    thread_cam.detach();
    thread_loop.detach();
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
    //if (this->startButtonClicked_)
    //    return;
    QPushButton* button = (QPushButton*)sender();    
    int timer = this->ui.setTimerLineEdit->text().toInt();
    if (timer < 1)
        return;
    this->swSwitch_.initialSeconds_ = timer;
    //this->swSwitch_.initialSeconds_ = timer - (this->swSwitch_.originalSeconds_ - this->swSwitch_.initialSeconds_);
    //if (this->swSwitch_.initialSeconds_ < 0)
    //    this->swSwitch_.initialSeconds_ = 0;
    //qDebug() << "I AM HEREEEEEE\n" << this->swSwitch_.initialSeconds_ << '\n';

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