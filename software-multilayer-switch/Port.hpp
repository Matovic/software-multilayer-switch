#pragma once

#include "PortCounter.hpp"

// std
#include <string>
#include <deque>
#include <mutex>

// libtins
#include <tins/tins.h>

const uint32_t PORT1_INTERFACE = 12;
const uint32_t PORT2_INTERFACE = 23;
const std::string PORT1_IP("192.168.100.254");
const std::string PORT2_IP("192.168.100.253");
const Tins::HWAddress<6> PORT1_MAC("02:00:4C:4F:4F:50");
const Tins::HWAddress<6> PORT2_MAC("02:00:4C:4F:4F:50");


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

	std::mutex mtx;
	bool wait_;
	// Tins::IPv4Address ipAddr;

private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	std::string friendlyName_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;
	std::deque<Tins::PDU*> bufferPDU_;
	bool savePDU(Port*, Tins::PDU&);
	// void sendPDU(Port*, Tins::PDU&);
};