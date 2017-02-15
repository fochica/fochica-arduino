// EventHandlerConnectedSensorStateChange.h

#ifndef _EVENTHANDLERCONNECTEDSENSORSTATECHANGE_h
#define _EVENTHANDLERCONNECTEDSENSORSTATECHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when a sensor state changes and there is a client (or clients) connected.
// This indicates an informational messages to the user that system is in an optimal state.
class EventHandlerConnectedSensorStateChange : public IEventHandler
{
public:
	EventHandlerConnectedSensorStateChange() {};
	bool eventSensorStateChange(IEventHandlerState &state, seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState);

private:
};


#endif

