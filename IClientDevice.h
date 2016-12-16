// IClientDevice.h

#ifndef _ICLIENTDEVICE_h
#define _ICLIENTDEVICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IClient.h"
#include "IServer.h"

// Defines an interface of a client device, which is typically a BLE module connected to Arduino
class IClientDevice : public IClient
{
public:
	// state
	virtual void begin() = 0;
	virtual bool isConnected() = 0;
	// receiving
	virtual void setReceiverCallback(IServer * callback);
	virtual void work() = 0;
	// capabilities
	virtual bool isCanReceivePackets(); // is in a mode right now in which adapter can receive data from client
	virtual bool isListenLimited(); // does this device belong to a class where only one can listen at a time?
	virtual void listen();

protected:
	IServer * mServerCallback; // pointer to a callback interface for incoming data
};

#endif

