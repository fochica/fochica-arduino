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

// Defines an interface to a client, which is typically a smartphone connected over BLE.
class IClient
{
public:
	virtual bool sendTime(const PacketTime& packet) = 0;
	//virtual bool sendTechnicalData(float vcc, float carBatteryVoltage, float carBatteryCurrent, int freeRAM) = 0;
	//virtual bool sendSensorData(uint8_t sensorId, uint8_t seatId, SensorLocation::e location, SensorType::e type, float value) = 0;
};

#endif

