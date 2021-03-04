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

    std::thread th1(&MainWindow::captureTraffic, this, &this->port1_);
    std::thread th2(&MainWindow::captureTraffic, this, &this->port2_);
	    
    //th1.detach();
    //th2.detach();
	th1.join();
	th2.join();

	this->writeStatistics();
}

void MainWindow::clearButtonPressed()
{
    if (!this->startButtonClicked_) return;
    QPushButton* button = (QPushButton*)sender();
    //this->ui.textDisplay->setText("clear");
    this->port1_.clearIOStatistics();
    this->port2_.clearIOStatistics();

	this->writeStatistics();

    if (this->startButtonClicked_) this->startButtonClicked_ = false;
}

bool MainWindow::analyzeTraffic(Port* port, Tins::PDU& pdu)
{
	//this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
	//this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));

	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		port->getInputTraffic().incrementEthernetII();
		port->getOutputTraffic().incrementEthernetII();
	}
	catch (const std::exception&)
	{
		Tins::PacketSender sender;
		Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
		if (port->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
		sender.send(pdu, iface);

		// this->writeStatistics();

		return false;
	}

	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		port->getInputTraffic().incrementIP();
		port->getOutputTraffic().incrementIP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			port->getInputTraffic().incrementARP();
			port->getOutputTraffic().incrementARP();

			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (port->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			// this->writeStatistics();

			return false;
		}
		catch (const std::exception&)
		{
			// this->mutex_mtx.lock();
			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (port->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			// this->writeStatistics();

			return false;
		}
	}

	try
	{
		const Tins::TCP& tcp = pdu.rfind_pdu<Tins::TCP>();
		port->getInputTraffic().incrementTCP();
		port->getOutputTraffic().incrementTCP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::UDP& udp = pdu.rfind_pdu<Tins::UDP>();
			port->getInputTraffic().incrementUDP();
			port->getOutputTraffic().incrementUDP();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();
				port->getInputTraffic().incrementICMP();
				port->getOutputTraffic().incrementICMP();
			}
			catch (const std::exception&)
			{
				Tins::PacketSender sender;
				Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
				if (port->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
				sender.send(pdu, iface);

				// this->writeStatistics();

				return false;
			}
		}
	}

	Tins::PacketSender sender;
	Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
	if (port->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
	sender.send(pdu, iface);

	// this->writeStatistics();

	return false;
}

void MainWindow::captureTraffic(Port* port)
{
    Tins::Sniffer sniffer(port->getNetworkInterface_().name());

    sniffer.sniff_loop(
        std::bind(
            &MainWindow::analyzeTraffic,
            this, port,
            std::placeholders::_1
        )
    );
}

void MainWindow::writeStatistics()
{
	std::lock_guard<std::mutex> lock(this->mutex_mtx);
	this->ui.textDisplay->setText(QString::fromStdString(this->port1_.getPortStatistics()));
	this->ui.textDisplay->append(QString::fromStdString(this->port2_.getPortStatistics()));
}