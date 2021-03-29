#pragma once

#include "Port.hpp"
#include "QThreadDisplayPDU.hpp"
//
//#include <deque>
//#include <tins/tins.h>

class SwSwitch
{
public:
    SwSwitch();
    SwSwitch(QThreadDisplayPDU& displayQThread);
    SwSwitch(SwSwitch& swSwitch);
    
    Port port1_;
    Port port2_;
    QThreadDisplayPDU displayQThread_;
//
//private:
//    std::deque<Tins::PDU> bufferPDU_;
};

