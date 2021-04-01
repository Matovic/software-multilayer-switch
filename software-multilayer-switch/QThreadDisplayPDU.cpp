#include "QThreadDisplayPDU.hpp"

#include <qtimer.h>
#include <QDebug>

QThreadDisplayPDU::QThreadDisplayPDU() : m_stop{ false }
{
}

QThreadDisplayPDU::QThreadDisplayPDU(QThreadDisplayPDU& write) : m_stop{ write.m_stop }
{
}

void QThreadDisplayPDU::run()
{
	// qDebug() << "From work thread: " << currentThreadId();
	sleep(1);
}

void QThreadDisplayPDU::stop()
{
	qDebug() << "Thread::stop called from main thread: " << currentThreadId();
	QMutexLocker locker(&m_mutex);
	m_stop = true;
}