#include "MainWindow.hpp"
#include "Port.hpp"

#include <thread>
//#include <QtCore>


MainWindow::MainWindow(SwSwitch& swSwitch, QWidget *parent)
    : QMainWindow(parent), swSwitch_{ swSwitch }
{
    ui.setupUi(this);

    // start button
    QPushButton* QPushButton_startButton = MainWindow::findChild<QPushButton*>("startButton");
    connect(QPushButton_startButton, SIGNAL(released()), this, SLOT(startButtonPressed()));

    // clear button
    QPushButton* QPushButton_closeButton = MainWindow::findChild<QPushButton*>("clearButton");
    connect(QPushButton_closeButton, SIGNAL(released()), this, SLOT(clearButtonPressed()));

    // connect close button to stop a thread
    connect(QPushButton_closeButton, SIGNAL(released()), &this->swSwitch_.displayQThread_, SLOT(stop()));

    // connect thread to a write PDU
    connect(&this->swSwitch_.displayQThread_, SIGNAL(finished()), this, SLOT(writePDU()));
}

void MainWindow::startButtonPressed()
{
    if (this->startButtonClicked_)
        return;
    this->startButtonClicked_ = true;
    QPushButton* button = (QPushButton*)sender();

    std::thread th1(&Port::captureTraffic, &this->swSwitch_.port1_, &this->swSwitch_.port2_);
    std::thread th2(&Port::captureTraffic, &this->swSwitch_.port2_, &this->swSwitch_.port1_);
    std::thread th3(&MainWindow::checkBuffer, this);
	    
    th1.detach();
    th2.detach();
    th3.detach();
}

void MainWindow::clearButtonPressed()
{
    if (!this->startButtonClicked_) 
        return;
    // else this->startButtonClicked_ = false;
    QPushButton* button = (QPushButton*)sender();

    this->swSwitch_.port1_.clearIOStatistics();
    this->swSwitch_.port2_.clearIOStatistics();

    this->writePDU();
/*	this->writeStatistics();
    this->writeCAM_Table();  */      
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
            this->swSwitch_.port1_.getBuffer().pop_front();
            this->swSwitch_.displayQThread_.start();
        }

        if (!this->swSwitch_.port2_.getBuffer().empty())
        {
            this->swSwitch_.port2_.getBuffer().pop_front();
            this->swSwitch_.displayQThread_.start();
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
	return "\tMAC\tPort\tTimer\n";
}

void MainWindow::writeCAM_Table()
{
    this->ui.camTableDisplay->setText(QString::fromStdString(this->getCAM_Table()));
}