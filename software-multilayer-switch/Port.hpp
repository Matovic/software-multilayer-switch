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

class Port
{
public:
	Port(const std::string&, const uint32_t&);
	Port(const Port& port);
	~Port();

	/*bool savePDU(Tins::PDU&);*/
	void clearIOStatistics();
	std::string getPortStatistics();

	Tins::NetworkInterface getNetworkInterface_();
	std::string getName();
	PortCounter& getInputTraffic();
	PortCounter& getOutputTraffic();
	std::deque<Tins::PDU*>& getBuffer();

	//bool savePDU(Port*, Tins::PDU&);
	void captureTraffic(Port*);


	// Tins::NetworkInterface networkInterface_;

private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;
	std::mutex mutex_mtx;
	std::deque<Tins::PDU*> bufferPDU_;

	bool savePDU(Port*, Tins::PDU&);
	void sendPDU(Port*, Tins::PDU&);
	//friend void captureTraffic(Port*);
};

//void captureTraffic(Port*);