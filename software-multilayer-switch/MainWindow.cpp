#include "MainWindow.hpp"

#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }
{
    ui.setupUi(this);

    // start button
    QPushButton* QPushButton_startButton = MainWindow::findChild<QPushButton*>("startButton");
    connect(QPushButton_startButton, SIGNAL(released()), this, SLOT(startButtonPressed()));

    // clear button
    QPushButton* QPushButton_closeButton = MainWindow::findChild<QPushButton*>("clearButton");
    connect(QPushButton_closeButton, SIGNAL(released()), this, SLOT(clearButtonPressed()));
}

void MainWindow::startButtonPressed()
{
    if (!this->startButtonClicked_) 
        this->startButtonClicked_ = true;
    QPushButton* button = (QPushButton*)sender();

    /*this->ui.statisticsDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
    this->ui.statisticsDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));*/

    //this->writeStatistics();

    std::thread th1(&Port::captureTraffic, &this->port1_, &this->port2_);
    //std::thread th2(&Port::captureTraffic, &this->port2_, &this->port1_);
	    
    th1.detach();
    //th2.detach();
	//th1.join();
	//th2.join();

	// whiel (citam buffer) { .... }

	this->writeStatistics();
    this->writeCAM_Table();
}

void MainWindow::clearButtonPressed()
{
    if (!this->startButtonClicked_) 
        return;
    QPushButton* button = (QPushButton*)sender();

    this->port1_.clearIOStatistics();
    this->port2_.clearIOStatistics();

	this->writeStatistics();
    this->writeCAM_Table();

    if (this->startButtonClicked_) 
        this->startButtonClicked_ = false;
}

void MainWindow::writeStatistics()
{
	//std::lock_guard<std::mutex> lock(this->mutex_mtx);
	this->ui.statisticsDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
	this->ui.statisticsDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));
}

std::string MainWindow::getCAM_Table()
{
	return "\tMAC\tPort\tTimer\n";
}

void MainWindow::writeCAM_Table()
{
    this->ui.camTableDisplay->setText(QString::fromStdString(this->getCAM_Table()));
}