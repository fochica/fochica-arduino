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
	sensorVal_t getValueInt(); // return in mV
	float getValueFloat(); // return in V
	int getSamplingTime();

private:
	// Possibly AVR specific
	// cross platform issue
#ifndef MUX4 // if simpler chips (Uno), such as 168/328
	const int BANDGAP_REF_PIN = 14; // 1110b
#else // if better chips (Mega), such as 1280/2560
	const int BANDGAP_REF_PIN = 14+16; // 11110b
#endif
	const float BANDGAP_VOLTAGE = 1.1;
	const int SETTLE_DURATION_US = 250;
};


#endif

