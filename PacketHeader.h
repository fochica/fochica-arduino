// PacketHeader.h

#ifndef _PACKETHEADER_h
#define _PACKETHEADER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketHeader {
	uint8_t length;
	uint8_t protocolVersion;
	uint8_t packetType;
};

#endif

