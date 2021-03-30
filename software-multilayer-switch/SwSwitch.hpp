#pragma once

#include "Port.hpp"
#include "QThreadDisplayPDU.hpp"

class SwSwitch
{
public:
    SwSwitch();
    SwSwitch(QThreadDisplayPDU& displayQThread);
    SwSwitch(SwSwitch& swSwitch);
    
    Port port1_;
    Port port2_;
    QThreadDisplayPDU displayQThread_;
};

