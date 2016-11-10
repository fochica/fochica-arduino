// SensorVcc.h

#ifndef _SENSORVCC_h
#define _SENSORVCC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that read the Vcc voltage of the controller
// Uses bandgap voltage reference. Measures the known bandgap value using Vcc as the reference. Calculates Vcc based on how the bandgap is sampled.
class SensorVcc : public ISensor
{
public:
	SensorVcc(const char * name);
	int getValueInt(); // return in mV
	float getValueFloat(); // return in V
	int getSamplingTime();

private:
	const int BANDGAP_REF_PIN = 14;
	const float BANDGAP_VOLTAGE = 1.1;
	const int SETTLE_DURATION_US = 250;
};


#endif

