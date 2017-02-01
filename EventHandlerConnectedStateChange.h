// EventHandlerConnectedStateChange.h

#ifndef _EVENTHANDLERCONNECTEDSTATECHANGE_h
#define _EVENTHANDLERCONNECTEDSTATECHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"
#include "SoundManager.h"

class EventHandlerConnectedStateChange : public IEventHandler
{
	bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);
};

#endif

