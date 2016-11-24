// PacketSensorData.h

#ifndef _PACKETSENSORDATA_h
#define _PACKETSENSORDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketSensorData {
	uint8_t sensorId;
	uint8_t seatId;
	uint8_t location; // SensorLocation::e
	uint8_t type; // SensorType::e
	uint16_t value;
	uint8_t state; // SensorState::e
};

#endif

