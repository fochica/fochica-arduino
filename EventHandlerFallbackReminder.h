// EventHandlerFallbackReminder.h

#ifndef _EVENTHANDLERFALLBACKREMINDER_h
#define _EVENTHANDLERFALLBACKREMINDER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"
#include "SoundManager.h"
#include "CalibratedSensor.h"

// this event handler is designed to provide a fallback alternative to alerting via client interface on disconnect.
// this mechanism will activate if no client is connected and the car engine is rurned off, it will then beep if any seat is occupied.
// doesn't work well for cars that turn the engine off automatically during stops. there will be many false positives.
// the passed CalibratedSensor should have State A as engine off and B as engine is running.
class EventHandlerFallbackReminder : public IEventHandler
{
public:
	EventHandlerFallbackReminder(CalibratedSensor & carEngineState);
	//bool eventSeatStateChange(IEventHandlerState &state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState);
	void work(IEventHandlerState &state);

private:
	CalibratedSensor & mCarEngineState;
	CalibratedSensorState::e mLastEngineState;
	bool mAlertActive;
	unsigned long mNextNotification;
	const unsigned long NOTIFICATION_INTERVAL = 10000; //ms

	const CalibratedSensorState::e ENGINE_OFF = CalibratedSensorState::A;
	const CalibratedSensorState::e ENGINE_RUNNING = CalibratedSensorState::B;

	bool isAnySeatNotEmpty(IEventHandlerState &state);
};

#endif

