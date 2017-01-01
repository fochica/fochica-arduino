// SensorSharpIRDistance.h

#ifndef _SENSORSHARPIRDISTANCE_h
#define _SENSORSHARPIRDISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// An analog IR distance sensor by Sharp.
// Such as GP2Y0A21YK0F or similar.
// Assumes analog reference is default and is at value specified by ISensor::DEFAULT_ADC_ANALOG_REFERENCE
class SensorSharpIRDistance : public ISensor
{
public:
	SensorSharpIRDistance(const char * name, uint8_t analogReadPin);
	sensorVal_t getValueInt(); // returns distance in mm
	float getValueFloat();
	int getSamplingTime();

private:
	uint8_t mReadPin;
	float mAnalogRef;

};

#endif

