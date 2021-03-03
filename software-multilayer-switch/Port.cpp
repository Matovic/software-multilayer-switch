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

bool Port::analyzeTraffic(Tins::PDU& pdu)
{
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		this->inputTraffic_.incrementEthernetII();
		this->outputTraffic_.incrementEthernetII();
	}
	catch (const std::exception&)
	{
		Tins::PacketSender sender;
		Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
		if (this->name_ == "port2")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
		sender.send(pdu, iface);

		return false;
	}

	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		this->inputTraffic_.incrementIP();
		this->outputTraffic_.incrementIP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			this->inputTraffic_.incrementARP();
			this->outputTraffic_.incrementARP();

			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (this->name_ == "port2")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			return false;
		}
		catch (const std::exception&)
		{
			Tins::PacketSender sender;
			Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
			if (this->name_ == "port2")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
			sender.send(pdu, iface);

			return false;
		}
	}

	try
	{
		const Tins::TCP& tcp = pdu.rfind_pdu<Tins::TCP>();
		this->inputTraffic_.incrementTCP();
		this->outputTraffic_.incrementTCP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::UDP& udp = pdu.rfind_pdu<Tins::UDP>();
			this->inputTraffic_.incrementUDP();
			this->outputTraffic_.incrementUDP();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();
				this->inputTraffic_.incrementICMP();
				this->outputTraffic_.incrementICMP();
			}
			catch (const std::exception&)
			{
				Tins::PacketSender sender;
				Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
				if (this->name_ == "port2")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
				sender.send(pdu, iface);

				return false;
			}
		}
	}
	Tins::PacketSender sender;
	Tins::NetworkInterface iface = Tins::NetworkInterface::from_index(PORT1_INTERFACE);
	if (this->name_ == "port2")	iface = Tins::NetworkInterface::from_index(PORT2_INTERFACE);
	sender.send(pdu, iface);

	return false;
}

void Port::clearIOStatistics()
{
	this->inputTraffic_.clearStatistics();
	this->outputTraffic_.clearStatistics();
}

std::string Port::getPortStatistics()
{
	return
		this->name_ + ":\n" + this->inputTraffic_.getIOtype()
		+ "\n\tEthernet II: " + std::to_string(this->inputTraffic_.getEthernetII())
		+ "\n\tARP: " + std::to_string(this->inputTraffic_.getARP())
		+ "\n\tIP: " + std::to_string(this->inputTraffic_.getIP())
		+ "\n\tUDP: " + std::to_string(this->inputTraffic_.getUDP())
		+ "\n\tTCP: " + std::to_string(this->inputTraffic_.getTCP())
		+ "\n\tICMP: " + std::to_string(this->inputTraffic_.getICMP())
		+ "\n\tHTTP: " + std::to_string(this->inputTraffic_.getHTTP())

		+ "\n" + this->outputTraffic_.getIOtype()
		+ "\n\tEthernet II: " + std::to_string(this->outputTraffic_.getEthernetII())
		+ "\n\tARP: " + std::to_string(this->outputTraffic_.getARP())
		+ "\n\tIP: " + std::to_string(this->outputTraffic_.getIP())
		+ "\n\tUDP: " + std::to_string(this->outputTraffic_.getUDP())
		+ "\n\tTCP: " + std::to_string(this->outputTraffic_.getTCP())
		+ "\n\tICMP: " + std::to_string(this->outputTraffic_.getICMP())
		+ "\n\tHTTP: " + std::to_string(this->outputTraffic_.getHTTP()) + "\n";
}

void captureTraffic(Port* port)
{
	Tins::Sniffer sniffer(port->networkInterface_.name());
	// Tins::Sniffer sniffer(Tins::NetworkInterface::default_interface().name());
	/* Uses the helper function to create a proxy object that
	 * will call this->handle. If you're using boost or C++11,
	 * you could use boost::bind or std::bind, that will also
	 * work.
	 */
	 sniffer.sniff_loop(
	 	std::bind(
	 		&Port::analyzeTraffic,
	 		port,
	 		std::placeholders::_1
	 	)
	 );
}