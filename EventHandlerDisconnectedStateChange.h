// EventHandlerDisconnectedStateChange.h

#ifndef _EVENTHANDLERDISCONNECTEDSTATECHANGE_h
#define _EVENTHANDLERDISCONNECTEDSTATECHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when a seat state changes and there is no client connected.
// This indicates a warning to the user that system is not in an optimal state.
class EventHandlerDisconnectedStateChange : public IEventHandler
{
public:
	EventHandlerDisconnectedStateChange(bool includePseudoChanges = false) : mIncludePseudoChanges(includePseudoChanges) {}
	bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);

private:
	bool mIncludePseudoChanges;
};

#endif

