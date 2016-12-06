// PacketLogicalData.h

#ifndef _PACKETLOGICALDATA_h
#define _PACKETLOGICALDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketLogicalData {
	uint32_t deviceUniqueId; // unique device id (not guaranteed to be persistent across boots)
	uint8_t clientCount; // number of adapters
	uint8_t connectedClientCount; // number of clients connected
	uint8_t seatCount;
	uint8_t sensorCount;
};

#endif

