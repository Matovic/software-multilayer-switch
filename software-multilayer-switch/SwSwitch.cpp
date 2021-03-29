#include "SwSwitch.hpp"

SwSwitch::SwSwitch() 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ }
{
}

SwSwitch::SwSwitch(QThreadDisplayPDU& write) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ write }
{
}

SwSwitch::SwSwitch(SwSwitch& swSwitch) 
	: port1_{ "port1", PORT1_INTERFACE }, port2_{ "port2", PORT2_INTERFACE }, displayQThread_{ swSwitch.displayQThread_ }
{
}

