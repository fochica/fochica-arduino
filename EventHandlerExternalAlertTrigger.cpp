// 
// 
// 

#include "EventHandlerExternalAlertTrigger.h"
#include "DebugStream.h"

EventHandlerExternalAlertTrigger::EventHandlerExternalAlertTrigger(CalibratedSensor & carEngineState, unsigned long alertDelay, uint8_t triggerPin, uint8_t offLevel, uint8_t onLevel) : mCarEngineState(carEngineState), 
	mAlertDelay(alertDelay), mTriggerPin(triggerPin), mOffLevel(offLevel), mOnLevel(onLevel)
{
	mAlertActive = false;
	mCountdownRunning = false;
}

void EventHandlerExternalAlertTrigger::begin()
{
	pinMode(mTriggerPin, OUTPUT);
	digitalWrite(mTriggerPin, mOffLevel);
}

void EventHandlerExternalAlertTrigger::work(IEventHandlerState & state)
{
	bool concerningState = state.isConnected() == false && mCarEngineState.getValue() == ENGINE_OFF && state.isAnySeatNotEmpty();

	if (concerningState) {
		if (mAlertActive == false) { // if alert not already active, otherwise nothing to do
			if (mCountdownRunning==false) { // if countdown not running
				mCountdownStart = millis(); // start
				mCountdownRunning = true;
				if (DebugStream) DebugStream->println(F("ExternalAlertTrigger, starting countdown"));
			}
			else if (millis() > mCountdownStart + mAlertDelay) { // if delay passed
				mAlertActive = true;
				digitalWrite(mTriggerPin, mOnLevel);
				if (DebugStream) DebugStream->println(F("ExternalAlertTrigger, starting alert"));
			}
		}
	} else {
		// stop countdown
		mCountdownRunning = false;
		// stop alert
		if (mAlertActive) {
			digitalWrite(mTriggerPin, mOffLevel);
			mAlertActive = false;
			if (DebugStream) DebugStream->println(F("ExternalAlertTrigger, stopping alert"));
		}
	}
}
