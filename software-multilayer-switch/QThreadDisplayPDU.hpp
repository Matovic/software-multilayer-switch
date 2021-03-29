#pragma once

//#include <qthread.h>
//#include <QThread>
//#include <QTimer>
//#include <QMetaObject>
//#include <QMutexLocker>
//#include <QMutex>
#include <QtWidgets>
//#include "Port.hpp"

//class Worker : public QObject
//{
//    Q_OBJECT
//private slots:
//    void onTimeout()
//    {
//        qDebug() << "Worker::onTimeout get called from?: " << QThread::currentThreadId();
//    }
//};

class QThreadDisplayPDU : public QThread
{
    Q_OBJECT
public:
    QThreadDisplayPDU();
    QThreadDisplayPDU(QThreadDisplayPDU& write);
public slots:
    void stop();
protected:
	void run();
private:
    QMutex m_mutex;
    bool m_stop;
};

