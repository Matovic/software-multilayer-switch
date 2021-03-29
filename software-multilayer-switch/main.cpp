#include "MainWindow.hpp"
#include "SwSwitch.hpp"
#include "QThreadDisplayPDU.hpp"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    qDebug() << "From main thread: " << QThread::currentThreadId();
    QThreadDisplayPDU displayQThread;
    QObject::connect(&displayQThread, SIGNAL(finished()), &a, SLOT(quit()));
    SwSwitch swSwitch(displayQThread);

    MainWindow w{ swSwitch };
    w.show();
    return a.exec();
}
