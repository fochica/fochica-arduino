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
	SensorDigital(const char * name, uint8_t pin, int8_t pinMode=-1); // can optionlly define pin mode (INPUT, INPUT_PULLUP or -1 for keep-as-is)
	void begin();

	int getValueInt(); // return 0 or 1*GAIN
	float getValueFloat(); // return 0 or 1
	int getSamplingTime();

private:
	uint8_t mPin;
	int8_t mPinMode;

	const int GAIN = 1000; // scale response to better use int range
};

#endif

