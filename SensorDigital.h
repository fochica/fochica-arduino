// SensorDigital.h

#ifndef _SENSORDIGITAL_h
#define _SENSORDIGITAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

class SensorDigital : public ISensor
{
public:
	SensorDigital(const char * name, int pin);
	int getValueInt();
	float getValueFloat();
	int getSamplingTime();

private:
	int mPin;

};

#endif

