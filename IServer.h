// IServer.h

#ifndef _ISERVER_h
#define _ISERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketTime.h"
#include "PacketSeatOperation.h"

// Defines an interface to the server deivce (the Arduino MCU). The interface is used to get packets from client devices.
class IServer
{
public:
	virtual bool receiveTime(const PacketTime& packet) = 0;
	virtual bool receiveSeatOperation(const PacketSeatOperation& packet) = 0;
};

#endif

