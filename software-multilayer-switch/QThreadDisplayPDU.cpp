#include "QThreadDisplayPDU.hpp"

#include <qtimer.h>
#include <QDebug>

QThreadDisplayPDU::QThreadDisplayPDU() : m_stop{ false }//: QThread()
{
	// We have to do this to make sure our thread has the
	// correct affinity.
	//moveToThread(this);
}

QThreadDisplayPDU::QThreadDisplayPDU(QThreadDisplayPDU& write) : m_stop{ write.m_stop }
{
	//moveToThread(this);
}

void QThreadDisplayPDU::run()
{
	//if (m_stop)
	//{
	//	m_stop = false;
	//	return;
	//}
	qDebug() << "From work thread: " << currentThreadId();
	sleep(5);
	//QTimer timer;
	//Worker worker;
	//connect(&timer, SIGNAL(timeout()), &worker, SLOT(onTimeout()));
	//timer.start(1000);

	//exec();
	//qDebug() << "From worker thread: " << currentThreadId();
	//while (1) 
	//{
	//	QMutexLocker locker(&m_mutex);
	//	if (m_stop)
	//	{
	//		m_stop = false;
	//		break;
	//	}
	//	msleep(10);
	//}
	//QTimer* timer = new QTimer();
	//connect(timer, SIGNAL(timeout()),
	//	this, SLOT(doTheWork(port1, port2)));
	//timer->start(0);

	//exec(); // starts the event loop, and doesn't
	//		// return until it is told to quit()
	
	//// This schedules the doTheWork() function
	//// to run just after our event loop starts up
	//QTimer::singleShot(0, this, SLOT(doTheWork()));

	//// This starts the event loop. Note that
	//// exec() does not return until the
	//// event loop is stopped.
	//exec();
}

//void QThreadDisplayPDU::doTheWork()
//{
//	//if (!port1.getBuffer().empty())
//	//{
//	//	port1.getBuffer().pop_front();
//	//	// this->writeStatistics();
//	//	// this->writeCAM_Table();
//	//}
//
//	//if (!port2.getBuffer().empty())
//	//{
//	//	port2.getBuffer().pop_front();
//	//	// this->writeStatistics();
//	//	// this->writeCAM_Table();
//	//}
//	//// Do the heavy-duty blocking stuff here
//	//// (simulated by a 5 second sleep for
//	//// this example)
//	//sleep(5);
//
//	//// When you're done, emit the results:
//	//emit done("First job's finished.");
//
//	//// And some more sleeping for fun
//	//sleep(3);
//	//emit done("Second job's finished.");
//
//	// ...
//}
//
void QThreadDisplayPDU::stop()
{
	qDebug() << "Thread::stop called from main thread: " << currentThreadId();
	QMutexLocker locker(&m_mutex);
	m_stop = true;
	//if (currentThread() != this)
	//{
	//	// The caller is running in a
	//	// different thread, so use Qt to
	//	// call stop() later, on our own thread:
	//	QMetaObject::invokeMethod(this, "stop",
	//		Qt::QueuedConnection);
	//}
	//else
	//{
	//	// Now the call has arrived from our
	//	// own thread, yay! We can safely
	//	// shut down our event loop.
	//	quit();
	//}
}