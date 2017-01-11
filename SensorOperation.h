// SensorOperation.h

#ifndef _SENSOROPERATION_h
#define _SENSOROPERATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct SensorOperation {
	enum e { SetActivityModeActive, SetActivityModeDeactivated, SetActivityModeDisabled
	};
};

#endif

