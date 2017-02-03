// EventHandlerExternalAlertTrigger.h

#ifndef _EVENTHANDLEREXTERNALALERTTRIGGER_h
#define _EVENTHANDLEREXTERNALALERTTRIGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"
#include "CalibratedSensor.h"

// this event handler is designed to trigger an external module in case of an alert. can be used to trigger a cellular device, a relay that will turn on siren, flash lights, open windows, etc.
// for the pin to trigger the system has to be in the folowing state for "alertDelay" milliseconds: no clients connected, engine not running, at least one seat is not empty.
// have to call begin() to initialize. until the call to begin the pin might have undefined value, please handle this in your circuit and preferably use the default level to mean "alarm off".
// doesn't work well for cars that turn the engine off automatically during stops. there will be many false positives.
// the passed CalibratedSensor should have State A as engine off and B as engine is running.
// you can implement your own IEventHandler or inherit from this implementation if you want to expand your external trigger or pass more information to the external module.
class EventHandlerExternalAlertTrigger : public IEventHandler
{
public:
	EventHandlerExternalAlertTrigger(CalibratedSensor & carEngineState, unsigned long alertDelay, uint8_t triggerPin, uint8_t offLevel=LOW, uint8_t onLevel=HIGH);
	void begin();
	void work(IEventHandlerState &state);

private:
	CalibratedSensor & mCarEngineState;
	unsigned long mAlertDelay;
	uint8_t mTriggerPin, mOffLevel, mOnLevel;

	bool mAlertActive; // are we outputting "alert on" to the pin
	bool mCountdownRunning;
	unsigned long mCountdownStart;

	const CalibratedSensorState::e ENGINE_OFF = CalibratedSensorState::A;
	const CalibratedSensorState::e ENGINE_RUNNING = CalibratedSensorState::B;
};

#endif

