/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
// this mechanism will activate if no client is connected and the car engine is turned off, it will then beep if any seat is occupied.
// doesn't work well for cars that turn the engine off automatically during stops. there will be many false positives.
// the passed CalibratedSensor should have State A as engine off and B as engine is running.
class EventHandlerFallbackReminder : public IEventHandler
{
public:
	EventHandlerFallbackReminder(CalibratedSensor & carEngineState);
	void work(IEventHandlerState &state);

private:
	CalibratedSensor & mCarEngineState;
	CalibratedSensorState::e mLastEngineState;
	bool mAlertActive;
	unsigned long mNextNotification;
	const unsigned long NOTIFICATION_INTERVAL = 10000; //ms

	const CalibratedSensorState::e ENGINE_OFF = CalibratedSensorState::A;
	const CalibratedSensorState::e ENGINE_RUNNING = CalibratedSensorState::B;
};

#endif

