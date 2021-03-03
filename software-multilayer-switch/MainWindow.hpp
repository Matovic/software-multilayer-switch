#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "Port.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::MainWindowClass ui;
    std::string welcome_msg_ = "software multilayer switch";
    bool startButtonClicked_ = false;

    Port port1_;
    Port port2_;

private slots:
    void startButtonPressed();
    void clearButtonPressed();
};
