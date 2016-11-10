// PacketTime.h

#ifndef _PACKETTIME_h
#define _PACKETTIME_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// time is in utc, seconds since 1970. offset to local is in seconds.
struct PacketTime {
	uint32_t utcTime;
	int16_t offsetToLocal;
};

#endif

