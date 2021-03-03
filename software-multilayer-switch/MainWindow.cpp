#include "MainWindow.hpp"

#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), port1_{ "Port 1", PORT1_INTERFACE }, port2_{ "Port 2", PORT2_INTERFACE }
{
    ui.setupUi(this);
    ui.textDisplay->setText(QString::fromStdString(this->welcome_msg_));

    // start button
    QPushButton* QPushButton_startButton = MainWindow::findChild<QPushButton*>("startButton");
    connect(QPushButton_startButton, SIGNAL(released()), this, SLOT(startButtonPressed()));

    // clear button
    QPushButton* QPushButton_closeButton = MainWindow::findChild<QPushButton*>("clearButton");
    connect(QPushButton_closeButton, SIGNAL(released()), this, SLOT(clearButtonPressed()));
}

void MainWindow::startButtonPressed()
{
    if (!this->startButtonClicked_) this->startButtonClicked_ = true;
    QPushButton* button = (QPushButton*)sender();

    this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
    this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));

    std::thread th1(captureTraffic, &this->port1_);
    std::thread th2(captureTraffic, &this->port2_);

    //auto f = [](MainWindow* window) {
    //    while (window->startButtonClicked_)
    //    {
    //        window->ui.textDisplay->setText(QString::fromStdString(window->port1_.getPortStatistics()));
    //        window->ui.textDisplay->append(QString::fromStdString(window->port2_.getPortStatistics()));
    //    }
    //};

    // This thread is launched by using  
    // lamda expression as callable 
    //std::thread th3(f, this);

    //while (this->startButtonClicked_)
    //{
    //    this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
    //    this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));
    //}

    // Wait for thread t1 to finish 
    //th1.join();
    // Wait for thread t2 to finish 
    //th2.join();

    this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
    this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));
}

void MainWindow::clearButtonPressed()
{
    if (!this->startButtonClicked_) return;
    QPushButton* button = (QPushButton*)sender();
    //this->ui.textDisplay->setText("clear");
    this->port1_.clearIOStatistics();
    this->port2_.clearIOStatistics();

    this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
    this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));

    if (this->startButtonClicked_) this->startButtonClicked_ = false;
}
