// EventHandlerConnectedStateChange.h

#ifndef _EVENTHANDLERCONNECTEDSTATECHANGE_h
#define _EVENTHANDLERCONNECTEDSTATECHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when a seat state changes and there is a client (or clients) connected.
// This indicates an informational messages to the user that system is in an optimal state.
class EventHandlerConnectedStateChange : public IEventHandler
{
public:
	EventHandlerConnectedStateChange(bool includePseudoChanges = false) : mIncludePseudoChanges(includePseudoChanges) {}
	bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);

private:
	bool mIncludePseudoChanges;
};

#endif

