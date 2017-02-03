// 
// 
// 

#include "EventHandlerFallbackReminder.h"
#include "DebugStream.h"

EventHandlerFallbackReminder::EventHandlerFallbackReminder(CalibratedSensor & carEngineState) : mCarEngineState(carEngineState)
{
	mAlertActive = false;
	mLastEngineState = ENGINE_OFF; // start with engine off, because there is no alert on going from "off" to "running"
}

void EventHandlerFallbackReminder::work(IEventHandlerState & state)
{
	// manage state
	CalibratedSensorState::e engineState = mCarEngineState.getValue();

	// debug
	/*if (DebugStream) {
		DebugStream->print("Fallback reminder, work, engineState: ");
		DebugStream->print(engineState);
		DebugStream->print(F(" anySeatNotEmpty: "));
		DebugStream->print(isAnySeatNotEmpty(state));
		DebugStream->println();
	}*/

	// start alert if we are not connected and any seat is occupied and the engine is turned off
	if (mAlertActive==false && mLastEngineState != engineState && engineState==ENGINE_OFF) {
		if (state.isConnected() == false && isAnySeatNotEmpty(state) == true) {
			// start alert
			mAlertActive = true;
			mNextNotification = millis();
			if (DebugStream) {
				DebugStream->println(F("Fallback reminder alert start"));
			}
		}
	}
	mLastEngineState = engineState; // save for next cycle

	// handle alert mode
	if (mAlertActive) {
		// stop if we got connected to a client or if all seats are empty or if car engine is running again
		if (state.isConnected() || isAnySeatNotEmpty(state) == false || engineState == ENGINE_RUNNING) {
			mAlertActive = false;
			if (DebugStream) {
				DebugStream->println(F("Fallback reminder alert stop"));
			}
		}
		else {
			// notify
			unsigned long now = millis(); // overflow every ~50 days
			if (now > mNextNotification) {
				SoundManager::getInstance().playBeep(BeepType::FallbackReminderAlert);
				mNextNotification += NOTIFICATION_INTERVAL;
			}
		}
	}
}

bool EventHandlerFallbackReminder::isAnySeatNotEmpty(IEventHandlerState & state)
{
	seatCount_t seats = state.getSeatCount();
	for (seatCount_t i = 0; i < seats; i++) {
		if (state.getSeatState(i) != SensorState::Empty)
			return true;
	}
	return false;
}
