#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "Port.hpp"

#include <mutex>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    void writeStatistics();

private:
    Ui::MainWindowClass ui;
    std::string welcome_msg_ = "software multilayer switch";
    bool startButtonClicked_ = false;

    Port port1_;
    Port port2_;

    std::mutex mutex_mtx;

    bool analyzeTraffic(Port*, Tins::PDU&);
    void captureTraffic(Port*);

private slots:
    void startButtonPressed();
    void clearButtonPressed();
};
