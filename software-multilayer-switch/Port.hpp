#pragma once

#include "PortCounter.hpp"

// std
#include <string>
#include <deque>

// libtins
#include <tins/tins.h>

const uint32_t PORT1_INTERFACE = 12;
const uint32_t PORT2_INTERFACE = 23;

class Port
{
public:
	Port(const std::string&, const uint32_t&);
	Port(const Port& port);
	~Port();
	void clearIOStatistics();
	std::string getPortStatistics();
	Tins::NetworkInterface getNetworkInterface_();
	std::string getFriendlyName();
	PortCounter& getInputTraffic();
	PortCounter& getOutputTraffic();
	std::deque<Tins::PDU*>& getBuffer();
	void captureTraffic(Port*);
private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	std::string friendlyName_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;
	std::deque<Tins::PDU*> bufferPDU_;
	bool savePDU(Port*, Tins::PDU&);
	void sendPDU(Port*, Tins::PDU&);
};