#pragma once

#include <string>

class PortCounter
{
public:
	PortCounter(const std::string&);
	~PortCounter();

	size_t getEthernetII();
	size_t getARP();
	size_t getIP();
	size_t getICMP();	
	size_t getUDP();
	size_t getTCP();
	size_t getHTTP();

	void incrementEthernetII();
	void incrementARP();
	void incrementUDP();
	void incrementTCP();
	void incrementIP();
	void incrementICMP();
	void incrementHTTP();
	void clearStatistics();

	std::string getIOtype();

private:
	std::string IOtype_;

	size_t Ethernet_II_;
	size_t ARP_;
	size_t IP_;
	size_t ICMP_;	
	size_t UDP_;
	size_t TCP_;
	size_t HTTP_;
};