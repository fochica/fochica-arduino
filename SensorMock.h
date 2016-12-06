// SensorMock.h

#ifndef _SENSORMOCK_h
#define _SENSORMOCK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

struct SensorMockType {
	enum e {
		Saw, // goes from min to max to min with a linear function (param=step size)
		Sine, // goes from min to max to min with a sine function (param=step size in milli 2*pi radians)
		Uniform, // returns sampes from a uniform distribution between in and max
		Normal // returns normal distribution between min and max
	};
};

class SensorMock : public ISensor
{
public:
	SensorMock(const char * name, SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param);
	void setParams(SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param);
	void resetState();

	sensorVal_t getValueInt(); // return in mV
	float getValueFloat(); // return in V
	int getSamplingTime();

private:
	// params
	SensorMockType::e mType;
	sensorVal_t mMin, mMax, mParam;

	// state
	sensorVal_t mCurValue;
	bool mCurDirUp;

	// const
	static const int SINE_STEPS = 1000; // number of sine steps we take in 2pi rads
};

#endif

