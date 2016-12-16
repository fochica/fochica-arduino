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

bool IClientDevice::isListenLimited()
{
	return false;
}

void IClientDevice::listen()
{
}
