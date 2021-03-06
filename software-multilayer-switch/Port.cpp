#include "Port.hpp"
#include "CiscoDiscoveryProtocol.hpp"

#include <functional>
#include <exception>
#include <tins/pdu_allocator.h>
#include <tins/loopback.h>

#include <QDebug>

Port::Port(const std::string& friendlyName, const uint32_t& portInterface)
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ friendlyName },
	networkInterface_{ Tins::NetworkInterface::from_index(portInterface) }, wait_{ false }
{
	config_.set_promisc_mode(true);
	config_.set_immediate_mode(true);
}

Port::Port(const Port& port) 
	: inputTraffic_{ "input" }, outputTraffic_{ "output" }, friendlyName_{ port.friendlyName_ },
	networkInterface_{ Tins::NetworkInterface::from_index(port.networkInterface_) }, wait_{ false }
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

std::string Port::getFilter()
{
	std::string str = this->getFriendlyName() + "\n";
	int i = 0;
	for (auto& filter : this->v_filters_)
	{
		str += "\n";
		str += std::to_string(i);

		str += "\nsrc ip add: ";
		str += filter.filter_src_ip_add_;	

		str += "\ndst ip add: ";
		str += filter.filter_dst_ip_add_;

		str += "\nfilter_src_mac_add: ";
		str += filter.filter_src_mac_add_;

		str += "\nfilter_dst_mac_add: ";
		str += filter.filter_dst_mac_add_;

		str += "\nfilter_src_port: ";
		str += std::to_string(filter.filter_src_port);

		str += "\nfilter_dst_port: ";
		str += std::to_string(filter.filter_dst_port);

		str += "\nProtocols: ";

		if (filter.b_http_)
			str += "HTTP ";
		if (filter.b_icmp_)
			str += "ICMP ";
		if (filter.b_tcp_)
			str += "TCP ";
		if (filter.b_udp_)
			str += "UDP ";
		if (filter.b_ip_)
			str += "IP ";
		if (filter.b_arp_)
			str += "ARP ";

		if (filter.b_filter_out)
			str += "\nOUT";										
		else 
			str += "\nIN";

		if (filter.b_filter_deny)
			str += "\nDENY";
		else
			str += "\PERMIT";	
		++i;
	}
	return str;
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
	for (auto& filter : this->v_filters_)
	{
		// filter set to deny everything
		if (filter.b_filter_deny && (filter.filter_src_ip_add_ == "any" ||
			filter.filter_src_mac_add_ == "any" || filter.filter_dst_ip_add_ == "any" || filter.filter_dst_mac_add_ == "any"))
		{
			return true;
		}
	}

	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();

		// filter
		Tins::HWAddress<6> dst_addr = eth.dst_addr();
		Tins::HWAddress<6> src_addr = eth.src_addr();
		for (auto& filter : this->v_filters_)
			if (filter.b_filter_deny && (dst_addr.to_string() == filter.filter_dst_mac_add_ ||
				src_addr.to_string() == filter.filter_src_mac_add_))
			{
				return true;
			}
		try
		{
			const Tins::Loopback& loopback = pdu.rfind_pdu<Tins::Loopback>();
			this->loopbackSeconds = 0;
			return true;
		}
		catch (const std::exception&)
		{
			this->getInputTraffic().incrementEthernetII();
			for (auto& filter : this->v_filters_)
			{
				if (filter.b_filter_deny) 
					continue;
				port->getOutputTraffic().incrementEthernetII();
			}

		}
	}
	catch (const std::exception&)
	{
		return true;
	}
	try
	{
		const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
		Tins::IPv4Address src_ip = ip.src_addr();
		Tins::IPv4Address dst_ip = ip.dst_addr();
		for (auto& filter : this->v_filters_)
		{
			if (filter.b_filter_deny && (src_ip.to_string() == filter.filter_src_ip_add_ || dst_ip.to_string() == filter.filter_dst_ip_add_))
			{
				return true;
			}
			if (filter.b_filter_deny && filter.b_ip_)
			{
				if (filter.b_filter_out)
					this->getInputTraffic().incrementIP();
				return true;
			}
			else if (filter.b_filter_deny && !filter.b_ip_)
			{
				return true;
			}
		}
		this->getInputTraffic().incrementIP();
		port->getOutputTraffic().incrementIP();
	}
	catch (const std::exception&)
	{
		try
		{
			const Tins::ARP& arp = pdu.rfind_pdu<Tins::ARP>();
			for (auto& filter : this->v_filters_)
			{
				if (filter.b_filter_deny && filter.b_arp_)
				{
					if (filter.b_filter_out)
						this->getInputTraffic().incrementARP();
					return true;
				}
				else if (!filter.b_filter_deny && !filter.b_arp_)
				{
					return true;
				}
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
		auto src_port = tcp.sport();
		auto dst_port = tcp.dport();

		for (auto& filter : this->v_filters_)
		{
			if (filter.b_filter_deny && (src_port == filter.filter_src_port || dst_port == filter.filter_dst_port))
			{
				return true;
			}

			if (filter.b_filter_deny && filter.b_tcp_)
			{
				if (filter.b_filter_out)
					this->getInputTraffic().incrementTCP();
				return true;
			}
			else if (!filter.b_filter_deny && !filter.b_tcp_)
			{
				return true;
			}
		}
		this->getInputTraffic().incrementTCP();
		port->getOutputTraffic().incrementTCP();

		if (tcp.dport() == 80 || tcp.sport() == 80)
		{
			for (auto& filter : this->v_filters_)
			{
				if (filter.b_filter_deny && filter.b_http_)
				{
					if (filter.b_filter_out)
						this->getInputTraffic().incrementHTTP();
					return true;
				}
				else if (!filter.b_filter_deny && !filter.b_http_)
				{
					return true;
				}
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
			auto src_port = udp.sport();
			auto dst_port = udp.dport();
			for (auto& filter : this->v_filters_)
			{
				if (filter.b_filter_deny && (src_port == filter.filter_src_port || dst_port == filter.filter_dst_port))
				{
					return true;
				}
				if (filter.b_filter_deny && filter.b_udp_)
				{
					if (filter.b_filter_out)
						this->getInputTraffic().incrementUDP();
					return true;
				}
				else if (!filter.b_filter_deny && !filter.b_udp_)
				{
					return true;
				}
			}
			this->getInputTraffic().incrementUDP();
			port->getOutputTraffic().incrementUDP();
		}
		catch (const std::exception&)
		{
			try
			{
				const Tins::ICMP& icmp = pdu.rfind_pdu<Tins::ICMP>();

				for (auto& filter : this->v_filters_)
				{
					if (filter.b_filter_deny && filter.b_icmp_)
					{
						if (filter.b_filter_out)
							this->getInputTraffic().incrementICMP();
						return true;
					}
					else if (!filter.b_filter_deny && !filter.b_icmp_)
					{
						return true;
					}
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
	Tins::Allocators::register_allocator<Tins::EthernetII, CiscoDiscoveryProtocol>(0x8ae);
	Tins::Sniffer sniffer(this->getNetworkInterface_().name(), this->config_);
	sniffer.sniff_loop(
		std::bind(
			&Port::savePDU,
			this, port2,
			std::placeholders::_1
		)
	);
}
