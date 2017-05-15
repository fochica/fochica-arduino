/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// EventHandlerWriteToPersistentLog.h

#ifndef _EVENTHANDLERWRITETOPERSISTENTLOG_h
#define _EVENTHANDLERWRITETOPERSISTENTLOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
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

