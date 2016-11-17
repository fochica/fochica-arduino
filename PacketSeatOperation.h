// PacketSeatOperation.h

#ifndef _PACKETSEATOPERATION_h
#define _PACKETSEATOPERATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketSeatOperation {
	uint8_t seatId;
	uint8_t operationId; // SeatOperation::e
};

#endif

