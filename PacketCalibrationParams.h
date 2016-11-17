// PacketCalibrationParams.h

#ifndef _PACKETCALIBRATIONPARAMS_h
#define _PACKETCALIBRATIONPARAMS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketCalibrationParams {
	uint8_t sensorId;
	uint8_t seatId;
	uint16_t triggerHigh;
	uint16_t triggerLow;
	uint16_t lowPassAlpha; // in milli units
	uint8_t highIsOccupied; // boolean
};

#endif

