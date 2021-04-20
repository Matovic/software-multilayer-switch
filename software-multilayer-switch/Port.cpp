#include "Port.hpp"

#include <functional>
#include <exception>
#include <tins/loopback.h>

#include <QDebug>

Port::Port(const std::string& friendlyName, const uint32_t& portInterface)
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ friendlyName },
	networkInterface_{ Tins::NetworkInterface::from_index(portInterface) }, wait_{ false }, b_filter_protocol_{ false }, 
	b_http_{ false }, b_icmp_{ false }, b_tcp_{ false }, b_udp_{ false }, b_ip_{ false }, b_arp_{ false }, port_number_{ 0 }
{
	config_.set_promisc_mode(true);
	config_.set_immediate_mode(true);
}

Port::Port(const Port& port) 
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ port.friendlyName_ },
	networkInterface_{ Tins::NetworkInterface::from_index(port.networkInterface_) }, wait_{ false }, b_filter_protocol_{ false },
	b_http_{ false }, b_icmp_{ false }, b_tcp_{ false }, b_udp_{ false }, b_ip_{ false }, b_arp_{ false }, port_number_{ 0 }
{
	config_.set_promisc_mode(true);
	config_.set_immediate_mode(true);
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

bool Port::savePDU(Port* port, Tins::PDU& pdu)
{
	/*if (this->b_filter_protocol_)
	{
		qDebug() << "set filters!\n";
		return false;
	}*/
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		try
		{
			const Tins::Loopback& loopback = pdu.rfind_pdu<Tins::Loopback>();
			this->loopbackSeconds = 0;
			return true;
		}
		catch (const std::exception&)
		{
			this->getInputTraffic().incrementEthernetII();
			if (this->port_number_ != 0 && !this->b_filter_deny)
				port->getOutputTraffic().incrementEthernetII();
		}
	}
	catch (const std::exception&)
	{
		return true;
	}
	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		if (this->port_number_ != 0 && this->b_filter_deny && this->b_ip_)
		{
			if (this->b_filter_out)
				this->getInputTraffic().incrementIP();
			qDebug() << "\tIP\n";
			return true;
		}
		else if(this->port_number_ != 0 && !this->b_filter_deny && !this->b_ip_)
		{
			return true;
		}
		this->getInputTraffic().incrementIP();
		port->getOutputTraffic().incrementIP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			if (this->port_number_ != 0 && this->b_filter_deny && this->b_arp_)
			{
				if (this->b_filter_out)
					this->getInputTraffic().incrementARP();
				qDebug() << "\tARP\n";
				return true;
			}
			else if (this->port_number_ != 0 && !this->b_filter_deny && !this->b_arp_)
			{
				return true;
			}
			this->getInputTraffic().incrementARP();
			port->getOutputTraffic().incrementARP();

			auto bufferSerialize = pdu.serialize();
			uint hash_ = qHash(bufferSerialize, QCryptographicHash::Md5);

			auto it = std::find(hashMap_.begin(), hashMap_.end(), hash_);
			if (it != hashMap_.end())
			{
				qDebug() << "ARP rovnaky" << this->friendlyName_.c_str() << " " << hash_ << '\n';
				return true;
			}
			else
			{
				qDebug() << "ARP push back" << this->friendlyName_.c_str() << " " << hash_ << '\n';
				hashMap_.push_back(hash_);
			}
			this->bufferPDU_.push_back(pdu.clone());
			return true;
		}
		catch (const std::exception&)
		{
			return true;
		}
	}
	try
	{
		const Tins::TCP& tcp = pdu.rfind_pdu<Tins::TCP>();
		if (this->port_number_ != 0 && this->b_filter_deny && this->b_tcp_)
		{
			if (this->b_filter_out)		
				this->getInputTraffic().incrementTCP();
			qDebug() << "\tTCP\n";
			return true;
		}
		else if (this->port_number_ != 0 && !this->b_filter_deny && !this->b_tcp_)
		{
			return true;
		}
		this->getInputTraffic().incrementTCP();
		port->getOutputTraffic().incrementTCP();

		if (tcp.dport() == 80 || tcp.sport() == 80)
		{
			if (this->port_number_ != 0 && this->b_filter_deny && this->b_http_)
			{
				if (this->b_filter_out)
					this->getInputTraffic().incrementHTTP();
				qDebug() << "\tHTTP\n";
				return true;
			}
			else if (this->port_number_ != 0 && !this->b_filter_deny && !this->b_http_)
			{
				return true;
			}
			this->getInputTraffic().incrementHTTP();
			port->getOutputTraffic().incrementHTTP();
		}
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::UDP& udp = pdu.rfind_pdu<Tins::UDP>();
			if (this->port_number_ != 0 && this->b_filter_deny && this->b_udp_)
			{
				if (this->b_filter_out)			
					this->getInputTraffic().incrementUDP();
				qDebug() << "\tUDP\n";
				return true;
			}
			else if (this->port_number_ != 0 && !this->b_filter_deny && !this->b_udp_)
			{
				return true;
			}
			this->getInputTraffic().incrementUDP();
			port->getOutputTraffic().incrementUDP();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();
				if (this->port_number_ != 0 && this->b_filter_deny && this->b_icmp_)
				{
					if (this->b_filter_out)
						this->getInputTraffic().incrementICMP();
					qDebug() << "\tICMP\n";
					return true;
				}
				else if (this->port_number_ != 0 && !this->b_filter_deny && !this->b_icmp_)
				{
					return true;
				}
				this->getInputTraffic().incrementICMP();
				this->loopbackSeconds = 0;
				port->getOutputTraffic().incrementICMP();
			}
			catch (const std::exception&)
			{
				return true;
			}
		}
	}
	auto bufferSerialize = pdu.serialize();
	uint hash_ = qHash(bufferSerialize, QCryptographicHash::Md5);

	auto it = std::find(hashMap_.begin(), hashMap_.end(), hash_);
	if (it != hashMap_.end())
	{
		qDebug() << this->friendlyName_.c_str() << " " << hash_ << '\n';
		return true;
	}
	else
	{
		qDebug() << "push back "<<this->friendlyName_.c_str() << " " << hash_ << '\n';
		hashMap_.push_back(hash_);
	}
	this->bufferPDU_.push_back(pdu.clone());
	return true;
}

void Port::captureTraffic(Port* port2)
{
	Tins::Sniffer sniffer(this->getNetworkInterface_().name(), this->config_);
	qDebug() << "Name: " << this->getNetworkInterface_().name().c_str() << '\n';
	sniffer.sniff_loop(
		std::bind(
			&Port::savePDU,
			this, port2,
			std::placeholders::_1
		)
	);
	//if (this->b_filter_protocol_)
	//{
	//	this->b_filter_protocol_ = false;
	//	qDebug() << "woala!\n";
	//	this->captureTraffic(port2);
	//}
}
