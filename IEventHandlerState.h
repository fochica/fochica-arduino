// IEventHandlerState.h

#ifndef _IEVENTHANDLERSTATE_h
#define _IEVENTHANDLERSTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SensorManager.h"

// Interface for Event Handlers to get access to device state
class IEventHandlerState
{
public:
	virtual bool isConnected() =0;
	virtual seatCount_t getSeatCount() =0;
	virtual SensorState::e getSeatState(seatCount_t seatId) =0;

	// helper
	virtual bool isAnySeatNotEmpty();
};

#endif

