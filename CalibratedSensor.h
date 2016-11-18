// CalibratedSensor.h

#ifndef _CALIBRATEDSENSOR_h
#define _CALIBRATEDSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// A wrapper over a raw sensor input that implements cleanup and thresholding
// including automated process to derrive parameters for cleanup and thresholding
// In the future: persistence of calibration parameters
class CalibratedSensor
{
public:
	CalibratedSensor(ISensor * raw); // must provide a valid raw sensor as source

private:
	ISensor * mRaw;
};

#endif

