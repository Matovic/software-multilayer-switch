#include "Port.hpp"

#include <functional>
#include <exception>

Port::Port(const std::string& name, const uint32_t& port_interface)
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, name_{ name },
	networkInterface_{ Tins::NetworkInterface::from_index(port_interface) }
{

}

Port::~Port()
{

}

void Port::clearIOStatistics()
{
	this->inputTraffic_.clearStatistics();
	this->outputTraffic_.clearStatistics();
}

std::string Port::getPortStatistics()
{
	return
		this->name_ + "\n\t" + this->inputTraffic_.getIOtype()
		+ "\n\t\tEthernet II: " + std::to_string(this->inputTraffic_.getEthernetII())
		+ "\n\t\tARP: " + std::to_string(this->inputTraffic_.getARP())
		+ "\n\t\tIP: " + std::to_string(this->inputTraffic_.getIP())
		+ "\n\t\tUDP: " + std::to_string(this->inputTraffic_.getUDP())
		+ "\n\t\tTCP: " + std::to_string(this->inputTraffic_.getTCP())
		+ "\n\t\tICMP: " + std::to_string(this->inputTraffic_.getICMP())
		+ "\n\t\tHTTP: " + std::to_string(this->inputTraffic_.getHTTP())

		+ "\n\t" + this->outputTraffic_.getIOtype()
		+ "\n\t\tEthernet II: " + std::to_string(this->outputTraffic_.getEthernetII())
		+ "\n\t\tARP: " + std::to_string(this->outputTraffic_.getARP())
		+ "\n\t\tIP: " + std::to_string(this->outputTraffic_.getIP())
		+ "\n\t\tUDP: " + std::to_string(this->outputTraffic_.getUDP())
		+ "\n\t\tTCP: " + std::to_string(this->outputTraffic_.getTCP())
		+ "\n\t\tICMP: " + std::to_string(this->outputTraffic_.getICMP())
		+ "\n\t\tHTTP: " + std::to_string(this->outputTraffic_.getHTTP()) + "\n";
}

Tins::NetworkInterface Port::getNetworkInterface_()
{
	return this->networkInterface_;
}

std::string Port::getName()
{
	return this->name_;
}

PortCounter& Port::getInputTraffic()
{
	return this->inputTraffic_;
}

PortCounter& Port::getOutputTraffic()
{
	return this->outputTraffic_;
}


bool Port::analyzeTraffic(Port* port2, Tins::PDU& pdu)
{
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		this->getInputTraffic().incrementEthernetII();
		port2->getOutputTraffic().incrementEthernetII();
	}
	catch (const std::exception&)
	{
		Tins::PacketSender sender;
		Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
		if (this->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
		sender.send(pdu, iface);

		// this->writeStatistics();

		return false;
	}

	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		this->getInputTraffic().incrementIP();
		port2->getOutputTraffic().incrementIP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			this->getInputTraffic().incrementARP();
			port2->getOutputTraffic().incrementARP();

			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (this->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			// this->writeStatistics();

			return false;
		}
		catch (const std::exception&)
		{
			// this->mutex_mtx.lock();
			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (this->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			// this->writeStatistics();

			return false;
		}
	}

	try
	{
		const Tins::TCP& tcp = pdu.rfind_pdu<Tins::TCP>();
		this->getInputTraffic().incrementTCP();
		port2->getOutputTraffic().incrementTCP();

		if (tcp.dport() == 80 || tcp.sport() == 80)
		{
			this->getInputTraffic().incrementHTTP();
			port2->getOutputTraffic().incrementHTTP();
		}
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::UDP& udp = pdu.rfind_pdu<Tins::UDP>();
			this->getInputTraffic().incrementUDP();
			port2->getOutputTraffic().incrementUDP();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();
				this->getInputTraffic().incrementICMP();
				port2->getOutputTraffic().incrementICMP();
			}
			catch (const std::exception&)
			{
				Tins::PacketSender sender;
				Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
				if (this->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
				sender.send(pdu, iface);

				return false;
			}
		}
	}

	Tins::PacketSender sender;
	Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
	if (this->getName() == "port1")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
	sender.send(pdu, iface);

	return false;
}

void Port::captureTraffic(Port* port2)
{
	Tins::Sniffer sniffer(this->getNetworkInterface_().name());

	sniffer.sniff_loop(
		std::bind(
			&Port::analyzeTraffic,
			this, port2,
			std::placeholders::_1
		)
	);
}
