#include "SwSwitch.hpp"
#include <ctime>

SwSwitch::SwSwitch() 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ }, initialSeconds_{ 30 }, originalSeconds_{ 30 }
{
}

SwSwitch::SwSwitch(QThreadDisplayPDU& write) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ write }, initialSeconds_{ 30 }, originalSeconds_{ 30 }
{
}

SwSwitch::SwSwitch(SwSwitch& swSwitch) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ swSwitch.displayQThread_ }, initialSeconds_{ 30 }, originalSeconds_{ 30 }
{
}

void SwSwitch::sendPDU(Port& port, Tins::PDU& pdu)
{
	Tins::PacketSender sender;
	try
	{
	//Tins::IP* ip = pdu.find_pdu<Tins::IP>();
	//if (port.getFriendlyName() == "port1" && ip)
	//{
	//	//ip->dst_addr().to_string()
	//	//if (ip->dst_addr().to_string() == PORT1_IP)
	//	//	qDebug() << ip->dst_addr().to_string().c_str() << " == " << PORT1_IP.c_str() << '\n';
	//	//else
	//	//	qDebug() << ip->dst_addr().to_string().c_str() << " != " << PORT1_IP.c_str() << '\n';
	//}
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		Tins::HWAddress<6> dst_addr = eth.dst_addr();
		auto it = this->camTable_.find(dst_addr);
		if (it != this->camTable_.end())
		{
			// Tins::EthernetII frame = Tins::EthernetII() / eth. IP() / TCP() / RawPDU("foo");
			qDebug() << "Preposielanie: " << it->second.begin()->first.c_str() << " " << it->first.to_string().c_str() << '\n';
			if (it->second.begin()->first == "port2")
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT2_INTERFACE));
			else
				sender.send(pdu, Tins::NetworkInterface::from_index(PORT1_INTERFACE));
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
		//qDebug() << "\tBUG\n";
		if (port.getFriendlyName() == "port1")
			sender.send(pdu, Tins::NetworkInterface::from_index(PORT2_INTERFACE));
		else
			sender.send(pdu, Tins::NetworkInterface::from_index(PORT1_INTERFACE));
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
		if (this->camTable_.empty()) continue;

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