#pragma once

#include "PortCounter.hpp"

// std
#include <string>

// libtins
#include <tins/tins.h>

const uint32_t PORT1_INTERFACE = 11;
const uint32_t PORT2_INTERFACE = 21;

class Port
{
public:
	Port(const std::string&, const uint32_t&);
	~Port();

	/*bool analyzeTraffic(Tins::PDU&);*/
	void clearIOStatistics();
	std::string getPortStatistics();

	Tins::NetworkInterface getNetworkInterface_();
	std::string getName();
	PortCounter& getInputTraffic();
	PortCounter& getOutputTraffic();


	// Tins::NetworkInterface networkInterface_;

private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;

	//friend void captureTraffic(Port*);
};

//void captureTraffic(Port*);