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
	 int getValueInt();
	 float getValueFloat();
	 int getSamplingTime();
};

#endif

