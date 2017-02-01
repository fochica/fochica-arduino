// IEventHandler.h

#ifndef _IEVENTHANDLER_h
#define _IEVENTHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandlerState.h"

// Interface that Event Handers have to implement
class IEventHandler
{
public:
	virtual void work(IEventHandlerState &state) {};
	virtual bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState) {};
	virtual bool eventClientConnectionChange(IEventHandlerState &state, bool isAdapterConnected) {};
};

#endif

