// PacketTechnicalData.h

#ifndef _PACKETTECHNICALDATA_h
#define _PACKETTECHNICALDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Packet.h"

struct PacketTechnicalData {
	float32_t vcc;
	float32_t carBatteryVoltage;
	float32_t carBatteryCurrent;
	uint16_t freeRAM;
};

#endif

