#include "Port.hpp"

#include <functional>
#include <exception>
#include <tins/loopback.h>

#include <QDebug>

Port::Port(const std::string& friendlyName, const uint32_t& portInterface)
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ friendlyName },
	networkInterface_{ Tins::NetworkInterface::from_index(portInterface) }, wait_{ false }//, ipAddr{}
{
}

Port::Port(const Port& port) 
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ port.friendlyName_ },
	networkInterface_{ Tins::NetworkInterface::from_index(port.networkInterface_) }, wait_{ false }
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
		this->friendlyName_ + "\n\t" + this->inputTraffic_.getIOtype()
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

std::string Port::getFriendlyName()
{
	return this->friendlyName_;
}

PortCounter& Port::getInputTraffic()
{
	return this->inputTraffic_;
}

PortCounter& Port::getOutputTraffic()
{
	return this->outputTraffic_;
}

std::deque<Tins::PDU*>& Port::getBuffer()
{
	return this->bufferPDU_;
}

//void Port::sendPDU(Port* port, Tins::PDU& pdu)
//{
//	Tins::PacketSender sender;
//	if (this->getFriendlyName() == "port1")	
//		sender.send(pdu, Tins::NetworkInterface::from_index(PORT2_INTERFACE));
//	else
//		sender.send(pdu, Tins::NetworkInterface::from_index(PORT1_INTERFACE));
//}

bool Port::getHash(QByteArray hashValue)
{
	auto it = std::find(this->hashMap_.begin(), this->hashMap_.end(), hashValue);
	if (it != this->hashMap_.end())
	{
		qDebug() << this->friendlyName_.c_str() << " " << hashValue << '\n';
		return false;
	}
	else
	{
		this->hashMap_.push_back(hashValue);
	}
	//this->hashMap_.addData(hashValue);
	//qDebug() << this->hashMap_.result() << '\n';
	//if (this->hashMap_.contains(hashValue))
	//{
	//	qDebug() << this->hashMap_.key(hashValue) << '\n';
	//	return true;
	//}
	//else
	//{
	//	this->hashMap_.addData(hashValue);
	//}
	return true;
}

bool Port::savePDU(Port* port, Tins::PDU& pdu)
{
	QByteArray hashValue;
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		//auto hashValue = qHash(&eth, 69);
		try
		{
			const Tins::Loopback& loopback = pdu.rfind_pdu<Tins::Loopback>();
			return true;
		}
		catch (const std::exception&)
		{
			//this->wait_ = true;
			this->getInputTraffic().incrementEthernetII();
			port->getOutputTraffic().incrementEthernetII();
		}
		hashValue += (eth.dst_addr().to_string() + eth.src_addr().to_string()).c_str();
		//hashValue = QCryptographicHash::hash(QByteArray{ eth.dst_addr().to_string().c_str() }, QCryptographicHash::Md5);
		//hashValue += QCryptographicHash::hash(QByteArray{ eth.src_addr().to_string().c_str() }, QCryptographicHash::Md5);
	}
	catch (const std::exception&)
	{
		// this->sendPDU(port, pdu);
		//while (this->wait_);
		qDebug() << "Not EII: ";
		return true;
	}

	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		this->getInputTraffic().incrementIP();
		port->getOutputTraffic().incrementIP();
		hashValue += (std::to_string(ip.id()) + std::to_string(ip.ttl()) + std::to_string(ip.checksum()) + ip.src_addr().to_string() + ip.dst_addr().to_string()).c_str();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			this->getInputTraffic().incrementARP();
			port->getOutputTraffic().incrementARP();
			// this->sendPDU(port, pdu);
			hashValue += (arp.sender_hw_addr().to_string() + arp.sender_ip_addr().to_string() + arp.target_hw_addr().to_string() 
				+ arp.target_ip_addr().to_string() + std::to_string(arp.opcode())).c_str(); 
			hashValue = QCryptographicHash::hash(hashValue, QCryptographicHash::Md5);

			if (!this->getHash(hashValue))
				return true;

			this->bufferPDU_.push_back(pdu.clone());
			//while (this->wait_);
			return true;
		}
		catch (const std::exception&)
		{
			// this->sendPDU(port, pdu);
			//while (this->wait_);
			return true;
		}
	}

	try
	{
		const Tins::TCP& tcp = pdu.rfind_pdu<Tins::TCP>();
		this->getInputTraffic().incrementTCP();
		port->getOutputTraffic().incrementTCP();

		hashValue += (std::to_string(tcp.dport()) + std::to_string(tcp.sport()) + std::to_string(tcp.seq()) + std::to_string(tcp.ack_seq()) + std::to_string(tcp.window())
			+ std::to_string(tcp.checksum())).c_str();

		if (tcp.dport() == 80 || tcp.sport() == 80)
		{
			this->getInputTraffic().incrementHTTP();
			port->getOutputTraffic().incrementHTTP();
		}
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::UDP& udp = pdu.rfind_pdu<Tins::UDP>();
			this->getInputTraffic().incrementUDP();
			port->getOutputTraffic().incrementUDP();

			hashValue += (std::to_string(udp.dport()) + std::to_string(udp.sport()) + std::to_string(udp.length()) + std::to_string(udp.checksum())).c_str();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();
				this->getInputTraffic().incrementICMP();
				port->getOutputTraffic().incrementICMP();

				hashValue += (std::to_string(icmp.code()) + std::to_string(icmp.checksum()) + std::to_string(icmp.id()) + std::to_string(icmp.sequence())).c_str();
			}
			catch (const std::exception&)
			{
				// this->sendPDU(port, pdu);
				//while (this->wait_);
				return true;
			}
		}
	}
	// this->sendPDU(port, pdu);
	hashValue = QCryptographicHash::hash(hashValue, QCryptographicHash::Md5);
	if (!this->getHash(hashValue))
		return true;
	this->bufferPDU_.push_back(pdu.clone());
	//while (this->wait_);
	return true;
}

void Port::captureTraffic(Port* port2)
{

	Tins::Sniffer sniffer(this->getNetworkInterface_().name());
	qDebug() << "Name: " << this->getNetworkInterface_().name().c_str() << '\n';
	sniffer.sniff_loop(
		std::bind(
			&Port::savePDU,
			this, port2,
			std::placeholders::_1
		)
	);
}
