#pragma once

#include "Port.hpp"
#include "QThreadDisplayPDU.hpp"
#include <map>
#include <ctime>
#include <string>
#include <thread>
#include <mutex>
#include<tins/tins.h>

class SwSwitch
{
public:
    SwSwitch();
    SwSwitch(QThreadDisplayPDU& displayQThread);
    SwSwitch(SwSwitch& swSwitch);
    
    int initialSeconds_;
    int originalSeconds_;
    bool cdpButtonClicked_ = false;
    Port port1_;
    Port port2_;
    QThreadDisplayPDU displayQThread_;
    std::map<Tins::HWAddress<6>, std::map<std::string, std::clock_t>> camTable_;
    std::thread thread_cdp_;
    std::mutex mtx;

    void sendPDU(Port&, Tins::PDU&);
    void checkCAM(Port&, Tins::PDU&);
    void cdpSender();
    void clearCAM();
    void updateCAM();
    void checkLoopbackProtocol();
};

