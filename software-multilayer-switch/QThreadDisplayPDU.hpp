#pragma once

#include <QtWidgets>

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

