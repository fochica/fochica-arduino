// 
// 
// 

#include "IClientDevice.h"

void IClientDevice::setReceiverCallback(IServer * callback)
{
	mServerCallback = callback;
}

bool IClientDevice::isCanReceivePackets()
{
	return true; // default
}
