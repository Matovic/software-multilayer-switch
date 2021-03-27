#pragma once

#include "PortCounter.hpp"

// std
#include <string>

// libtins
#include <tins/tins.h>

const uint32_t PORT1_INTERFACE = 12;
const uint32_t PORT2_INTERFACE = 23;

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

	//bool analyzeTraffic(Port*, Tins::PDU&);
	void captureTraffic(Port*);


	// Tins::NetworkInterface networkInterface_;

private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;

	bool analyzeTraffic(Port*, Tins::PDU&);
	//friend void captureTraffic(Port*);
};

//void captureTraffic(Port*);