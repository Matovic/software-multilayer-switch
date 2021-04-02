#pragma once

#include "PortCounter.hpp"

// std
#include <string>
#include <deque>
#include <mutex>
#include <vector>

// libtins
#include <tins/tins.h>

// Qt
//#include <QHash>
#include <QCryptographicHash> 
#include <QHash> 

const uint32_t PORT1_INTERFACE = 12;
const uint32_t PORT2_INTERFACE = 23;
const std::string PORT1_IP("192.168.100.254");
const std::string PORT2_IP("192.168.100.253");
const Tins::HWAddress<6> PORT1_MAC("02:00:4C:4F:4F:50");
const Tins::HWAddress<6> PORT2_MAC("02:00:4C:4F:4F:50");


static std::vector<uint> hashMap_;

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
	int loopbackSeconds;
	bool wait_;
	//QHash<int, int> hashMap_;
	//QCryptographicHash hashMap_{ QCryptographicHash::Md5 };
	//std::vector<uint> hashMap_;

private:
	Tins::NetworkInterface networkInterface_;
	std::string name_;
	std::string friendlyName_;
	PortCounter inputTraffic_;
	PortCounter outputTraffic_;
	std::deque<Tins::PDU*> bufferPDU_;
	bool savePDU(Port*, Tins::PDU&);
	bool getHash(QByteArray);
	// void sendPDU(Port*, Tins::PDU&);
};

//inline uint qHash(Tins::PDU& value)
//{
//	return qHash(value);
//}

inline uint qHash(const Tins::PDU::serialization_type& key, uint seed = 0)		//const std::vector<unsigned char>&
{
	if (key.empty())
		return seed;
	else
		return qHashBits(&key.front(), key.size() * sizeof(int), seed);
}