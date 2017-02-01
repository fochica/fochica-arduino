// EventHandlerDisconnectedStateChange.h

#ifndef _EVENTHANDLERDISCONNECTEDSTATECHANGE_h
#define _EVENTHANDLERDISCONNECTEDSTATECHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"
#include "SoundManager.h"

class EventHandlerDisconnectedStateChange : public IEventHandler
{
	bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);
};

#endif

