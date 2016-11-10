// IClient.h

#ifndef _ICLIENT_h
#define _ICLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Manager.h"
#include "PacketTime.h"
#include "PacketTechnicalData.h"
#include "PacketSensorData.h"

// Defines an interface to a client, which is typically a smartphone connected over BLE.
class IClient
{
public:
	virtual bool sendTime(const PacketTime& packet) = 0;
	virtual bool sendTechnicalData(const PacketTechnicalData& packet) = 0;
	virtual bool sendSensorData(const PacketSensorData& packet) = 0;
};

#endif

