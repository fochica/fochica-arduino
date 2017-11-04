/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// IServer.h

#ifndef _ISERVER_h
#define _ISERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketType.h"
#include "PacketTime.h"
#include "PacketSeatOperation.h"
#include "PacketSensorOperation.h"
#include "IClientDevice.h"

// Defines an interface to the server device (the Arduino MCU). The interface is used to get packets from client devices.
class IServer
{
public:
	virtual void onClientConnectionChange(IClientDevice * device, bool isConnected) = 0;

	virtual bool receiveTime(IClientDevice * device, const PacketTime& packet) = 0;
	virtual bool receiveSeatOperation(IClientDevice * device, const PacketSeatOperation& packet) = 0;
	virtual bool receiveSensorOperation(IClientDevice * device, const PacketSensorOperation& packet) = 0;

	static int getPacketLength(PacketType::e type);
};

#endif

