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