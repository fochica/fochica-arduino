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
	uint16_t vcc; // mV
	uint16_t carBatteryVoltage; // mV
	uint16_t carBatteryCurrent; // mA
	uint16_t freeRAM;
};

#endif

