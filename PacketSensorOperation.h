// PacketSensorOperation.h

#ifndef _PACKETSENSOROPERATION_h
#define _PACKETSENSOROPERATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketSensorOperation {
	uint8_t sensorId;
	uint8_t operationId; // SensorOperation::e
};


#endif

