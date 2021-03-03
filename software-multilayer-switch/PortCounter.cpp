#include "PortCounter.hpp"

PortCounter::PortCounter(const std::string& IOtype)
	: IOtype_{ IOtype }, Ethernet_II_{ 0 }, ARP_{ 0 }, IP_{ 0 }, UDP_{ 0 }, TCP_{ 0 }, ICMP_{ 0 }, HTTP_{ 0 }
{

}

PortCounter::~PortCounter()
{

}

std::string PortCounter::getIOtype()
{
	return this->IOtype_;
}

size_t PortCounter::getEthernetII()
{
	return this->Ethernet_II_;
}

size_t PortCounter::getARP()
{
	return this->ARP_;
}

size_t PortCounter::getIP()
{
	return this->IP_;
}

size_t PortCounter::getUDP()
{
	return this->UDP_;
}

size_t PortCounter::getTCP()
{
	return this->TCP_;
}

size_t PortCounter::getICMP()
{
	return this->ICMP_;
}

size_t PortCounter::getHTTP()
{
	return this->HTTP_;
}

void PortCounter::incrementEthernetII()
{
	++this->Ethernet_II_;
}

void PortCounter::incrementARP()
{
	++this->ARP_;
}

void PortCounter::incrementIP()
{
	++this->IP_;
}

void PortCounter::incrementUDP()
{
	++this->UDP_;
}

void PortCounter::incrementTCP()
{
	++this->TCP_;
}

void PortCounter::incrementICMP()
{
	++this->ICMP_;
}

void PortCounter::incrementHTTP()
{
	++this->HTTP_;
}

void PortCounter::clearStatistics()
{
	this->Ethernet_II_ = 0;
	this->ARP_ = 0;
	this->IP_ = 0;
	this->UDP_ = 0;
	this->TCP_ = 0;
	this->ICMP_ = 0;
	this->HTTP_ = 0;
}