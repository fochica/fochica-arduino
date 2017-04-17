/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
		if (state.isConnected() == false && state.isAnySeatNotEmpty() == true) {
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
		if (state.isConnected() || state.isAnySeatNotEmpty() == false || engineState == ENGINE_RUNNING) {
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