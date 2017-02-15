// EventHandlerWriteToPersistentLog.h

#ifndef _EVENTHANDLERWRITETOPERSISTENTLOG_h
#define _EVENTHANDLERWRITETOPERSISTENTLOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when a sensor state changes and there is a client (or clients) connected.
// This indicates an informational messages to the user that system is in an optimal state.
class EventHandlerWriteToPersistentLog : public IEventHandler
{
public:
	EventHandlerWriteToPersistentLog() {};
	bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);
	bool eventSensorStateChange(IEventHandlerState &state, seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState);
	bool eventClientConnectionChange(IEventHandlerState &state, bool isAdapterConnected);

private:
};


#endif

