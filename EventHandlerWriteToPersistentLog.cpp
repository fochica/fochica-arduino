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

#include "EventHandlerWriteToPersistentLog.h"
#include "PersistentLog.h"

bool EventHandlerWriteToPersistentLog::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Seat state change, seat: "));
			file->print(seatId);
			file->print(F(", state: "));
			file->print(lastState);
			file->print(F(" -> "));
			file->println(newState);
			PersistentLog->close();
		}
	}
	return true;
}

bool EventHandlerWriteToPersistentLog::eventSensorStateChange(IEventHandlerState & state, seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Seat sensor state change, seat: "));
			file->print(seatId);
			file->print(F(", sensor: "));
			file->print(sensorId);
			file->print(F(", state: "));
			file->print(lastState);
			file->print(F(" -> "));
			file->println(newState);
			PersistentLog->close();
		}
	}
	return true;
}

bool EventHandlerWriteToPersistentLog::eventClientConnectionChange(IEventHandlerState & state, bool isAdapterConnected)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Adapter connection change, connected: "));
			file->println(isAdapterConnected);
			PersistentLog->close();
		}
	}
	return true;
}
