#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "SwSwitch.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(SwSwitch&, QWidget *parent = Q_NULLPTR);

private:
    Ui::MainWindowClass ui;
    SwSwitch swSwitch_;
    bool startButtonClicked_ = false;

    std::string getCAM_Table();
    void writeCAM_Table();
    void writeStatistics();
    void checkBuffer();
    //void wrapperDone(const QString&);

private slots:
    void startButtonPressed();
    void clearButtonPressed();
    void setTimerButtonPressed();
    void setFiltersButtonPressed();
    void writePDU();
};
