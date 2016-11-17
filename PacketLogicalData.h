// PacketLogicalData.h

#ifndef _PACKETLOGICALDATA_h
#define _PACKETLOGICALDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketLogicalData {
	uint8_t clientCount; // number of receivers
	uint8_t connectedClientCount; // number of receivers connected
	uint8_t seatCount;
	uint8_t sensorCount;
};

#endif

