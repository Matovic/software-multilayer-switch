#include "SwSwitch.hpp"
#include "CiscoDiscoveryProtocol.hpp"
#include <ctime>

SwSwitch::SwSwitch() 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ }, initialSeconds_{ 30 }, originalSeconds_{ 30 },
	thread_cdp_{ &SwSwitch::cdpSender, this }
{
}

SwSwitch::SwSwitch(QThreadDisplayPDU& write) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ write }, initialSeconds_{ 30 }, originalSeconds_{ 30 },
	thread_cdp_{ &SwSwitch::cdpSender, this }
{
}

SwSwitch::SwSwitch(SwSwitch& swSwitch) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ swSwitch.displayQThread_ }, initialSeconds_{ 30 }, originalSeconds_{ 30 },
	thread_cdp_{ &SwSwitch::cdpSender, this }
{
}

void SwSwitch::sendPDU(Port& port, Tins::PDU& pdu)
{
	for (auto& filter : port.v_filters_)
	{
		// filter set to deny everything
		if (filter.b_filter_deny && (filter.filter_src_ip_add_ == "any" ||
			filter.filter_src_mac_add_ == "any" || filter.filter_dst_ip_add_ == "any" || filter.filter_dst_mac_add_ == "any"))
		{
			return;
		}
	}
	Tins::PacketSender sender;
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		Tins::HWAddress<6> dst_addr = eth.dst_addr();
		Tins::HWAddress<6> src_addr = eth.src_addr();
		for (auto& filter : port.v_filters_)
			if (filter.b_filter_deny && (dst_addr.to_string() == filter.filter_dst_mac_add_ ||
				src_addr.to_string() == filter.filter_src_mac_add_))
			{
				return;
			}
		try
		{
			const Tins::IP& ip = pdu.rfind_pdu<Tins::IP>();
			Tins::IPv4Address src_ip = ip.src_addr();
			Tins::IPv4Address dst_ip = ip.dst_addr();

			for (auto& filter : port.v_filters_)
				if (filter.b_filter_deny &&	(src_ip.to_string() == filter.filter_src_ip_add_ || 
					dst_ip.to_string() == filter.filter_dst_ip_add_))
				{
					return;
				}
		}
		catch (std::exception&)
		{
			qDebug() << "\tcatch\n";
		}

		auto it = this->camTable_.find(dst_addr);
		if (it != this->camTable_.end())
		{
			if (it->second.begin()->first == "port2")
			{
				qDebug() << "Preposielanie: " << it->second.begin()->first.c_str() << " " << it->first.to_string().c_str() << '\n';
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT2_INTERFACE));
			}
			else
			{
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT1_INTERFACE));
			}
		}
		else
		{
			if (port.getFriendlyName() == "port1")
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT2_INTERFACE));
			else
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT1_INTERFACE));
		}
	}
	catch (std::exception&)
	{
		return;
	}
}

void SwSwitch::checkCAM(Port& port, Tins::PDU& pdu)
{
	// std::lock_guard<std::mutex> lock(port.mtx);
	try
	{
		std::lock_guard<std::mutex> lock(this->mtx);
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		Tins::HWAddress<6> src_addr = eth.src_addr();

		// qDebug() << dst_addr.to_string().c_str() << "\t" << PORT1_MAC
		if (src_addr.to_string() != PORT1_MAC.to_string())
		{
			auto it = this->camTable_.find(src_addr);
			if (it != this->camTable_.end())// || (this->camTable_.size() == 1))
				it->second.begin()->second = this->initialSeconds_;
			else
				this->camTable_[src_addr].insert(std::make_pair(port.getFriendlyName(), this->initialSeconds_));
		}
		if (eth.dst_addr().to_string() != PORT1_MAC.to_string())
			this->sendPDU(port, pdu);
	} 
	catch (const std::exception&)
	{
		// std::lock_guard<std::mutex> lock(this->mtx);
		//this->sendPDU(port, pdu);
		return;
	}
}

void SwSwitch::clearCAM()
{
	this->camTable_.clear();
}

void SwSwitch::updateCAM()
{
	while (1)
	{
		// if (this->camTable_.empty()) continue;

		std::clock_t start = std::clock();
		while (((std::clock() - start) / CLOCKS_PER_SEC) < 1);
		this->displayQThread_.start();
		this->displayQThread_.quit();
		for (auto it = this->camTable_.begin(); it != this->camTable_.end(); ++it)
		{
			bool isIteratorErased = false;
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				it2->second -= 1; 
				if (it2->second <= 0)
				{
					this->camTable_.erase(it);
					isIteratorErased = true;
					break;
				}
			}
			if (isIteratorErased)
				break;
		}
	}
}

void SwSwitch::cdpSender()
{
	while (1)
	{
		if (!this->cdpButtonClicked_) continue;
		// Tins::PacketSender sender;
		// CiscoDiscoveryProtocol pkt = CiscoDiscoveryProtocol(0x8ae, 40) / Tins::RawPDU("foo");
		// sender.send(pkt); 
		qDebug() << "CDP!\n";
	}
}

void SwSwitch::checkLoopbackProtocol()
{
	while (1)
	{
		// wait one second
		std::clock_t start = std::clock();
		while (((std::clock() - start) / CLOCKS_PER_SEC) < 1);
		++this->port1_.loopbackSeconds;
		++this->port2_.loopbackSeconds;
		if (this->port1_.loopbackSeconds > 5)
		{
			for (auto it = this->camTable_.begin(); it != this->camTable_.end(); ++it)
			{
				if (it->second.begin()->first == this->port1_.getFriendlyName())
				{
					this->camTable_.erase(it);
					break;
				}
			}
		}

		if (this->port2_.loopbackSeconds > 5)
		{
			for (auto it = this->camTable_.begin(); it != this->camTable_.end(); ++it)
			{
				if (it->second.begin()->first == this->port2_.getFriendlyName())
				{
					this->camTable_.erase(it);
					break;
				}
			}
		}
	}
}