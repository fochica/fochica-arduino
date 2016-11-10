// 
// 
// 

#include "IClientDevice.h"

void IClientDevice::setReceiverCallback(IServer * callback)
{
	mServerCallback = callback;
}
