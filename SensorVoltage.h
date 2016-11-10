// SensorVoltage.h

#ifndef _SENSORVOLTAGE_h
#define _SENSORVOLTAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that reads voltage by using an analog pin connected to a voltage divider
// Assumes analog reference is default and is at value specified by ISensor::DEFAULT_ADC_ANALOG_REFERENCE
class SensorVoltage : public ISensor
{
public:
	 SensorVoltage(const char * name, int analogPin, long resistorToGround, long resistorToVoltage);
	 int getValueInt(); // return in mV
	 float getValueFloat(); // return in V
	 int getSamplingTime();

private:
	int mPin;
	float mDivider;
};

#endif
