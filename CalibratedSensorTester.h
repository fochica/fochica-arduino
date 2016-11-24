// CalibratedSensorTester.h

#ifndef _CALIBRATEDSENSORTESTER_h
#define _CALIBRATEDSENSORTESTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CalibratedSensor.h"

// This class is used for testing of th calibrated sensor logic
// This is not used during normal operation but only during development and testing
class CalibratedSensorTester
{
public:
	bool runTests();

private:
	void getAndPrintSample(CalibratedSensor & cs);
};


#endif

