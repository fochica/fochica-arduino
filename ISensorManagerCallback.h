// ISensorManagerCallback.h

#ifndef _ISENSORMANAGERCALLBACK_h
#define _ISENSORMANAGERCALLBACK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SensorMisc.h"

class ISensorManagerCallback
{
public:
	virtual bool eventSeatStateChange(seatCount_t seatId, SensorState::e lastState, SensorState::e newState) = 0;
};

#endif

