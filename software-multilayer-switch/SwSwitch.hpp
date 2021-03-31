#pragma once

#include "Port.hpp"
#include "QThreadDisplayPDU.hpp"
#include <map>
#include <ctime>
#include <string>
#include<tins/tins.h>

class SwSwitch
{
public:
    SwSwitch();
    SwSwitch(QThreadDisplayPDU& displayQThread);
    SwSwitch(SwSwitch& swSwitch);
    
    Port port1_;
    Port port2_;
    QThreadDisplayPDU displayQThread_;
    std::map<Tins::HWAddress<6>, std::map<std::string, std::clock_t>> camTable_;

    void checkCAM(const Tins::PDU&);
};

