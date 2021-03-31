#include "SwSwitch.hpp"

SwSwitch::SwSwitch() 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ }
{
}

SwSwitch::SwSwitch(QThreadDisplayPDU& write) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ write }
{
}

SwSwitch::SwSwitch(SwSwitch& swSwitch) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ swSwitch.displayQThread_ }
{
}

void SwSwitch::checkCAM(const Tins::PDU& pdu)
{
	try
	{
		const Tins::EthernetII& eth = pdu.rfind_pdu<Tins::EthernetII>();
		Tins::HWAddress<6> dst_addr = eth.dst_addr();
		Tins::HWAddress<6> src_addr = eth.src_addr();
		this->camTable_.insert(std::make_pair(src_addr, std::map<std::string, std::clock_t>()));
		this->camTable_[src_addr].insert(std::make_pair(this->port1_.getFriendlyName(), std::clock()));
		auto it = this->camTable_.begin();
		auto it_second = it->second.begin();
		qDebug() << "First: " << it->first.to_string().c_str() << "Port:" << it_second->first.c_str() << " clock:" << it_second->second << '\n';
	} 
	catch (const std::exception&)
	{
		return;
	}
}