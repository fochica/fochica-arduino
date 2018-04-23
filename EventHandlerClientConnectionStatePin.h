/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// EventHandlerClientConnectionStatePin.h

#ifndef _EVENTHANDLERCLIENTCONNECTIONSTATEPIN_h
#define _EVENTHANDLERCLIENTCONNECTIONSTATEPIN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// This event handler will put out a signal to a pin where the signal represents the (aggregated) connection state of the device to clients
// Begin must be used with this handler
class EventHandlerClientConnectionStatePin : public IEventHandler
{
public:
	EventHandlerClientConnectionStatePin(uint8_t pin, uint8_t offLevel = LOW, uint8_t onLevel = HIGH);
	void begin();
	bool eventClientConnectionChange(IEventHandlerState &state, bool isAdapterConnected);

private:
	uint8_t mPin, mOffLevel, mOnLevel;
};


#endif

