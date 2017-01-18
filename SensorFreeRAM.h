// SensorFreeRAM.h

#ifndef _SENSORFREERAM_h
#define _SENSORFREERAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that reads the amount of available SRAM
class SensorFreeRAM : public ISensor
{
 public:
	 SensorFreeRAM(const char * name);
	 sensorVal_t getValueInt(); // return bytes
	 float getValueFloat(); // return bytes
	 int getSamplingTime();

	 void dumpSRAMContent(Stream &s);
	 void dumpSRAMBounds(Stream &s);
};

#endif

