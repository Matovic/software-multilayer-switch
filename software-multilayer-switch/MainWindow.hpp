#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "Port.hpp"

// #include <mutex>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    void writeStatistics();

private:
    Ui::MainWindowClass ui;
    bool startButtonClicked_ = false;

    Port port1_;
    Port port2_;

    // std::mutex mutex_mtx;

    std::string getCAM_Table();
    void writeCAM_Table();

private slots:
    void startButtonPressed();
    void clearButtonPressed();
};
