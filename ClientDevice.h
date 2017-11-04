/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ClientDevice.h

#ifndef _CLIENTDEVICE_h
#define _CLIENTDEVICE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IClientDevice.h"
#include "IServer.h"

// Defines a base implementation of a client device, which is typically a BLE module connected to Arduino
class ClientDevice : public IClientDevice
{
public:
	ClientDevice() : mServerCallback(NULL) {}
	// receiving
	virtual void setReceiverCallback(IServer * callback);
	// capabilities
	virtual bool isCanReceivePackets(); // is in a mode right now in which adapter can receive data from client
	virtual bool isListenLimited(); // does this device belong to a class where only one can listen at a time?
	virtual void listen();

protected:
	IServer * mServerCallback; // pointer to a callback interface for incoming data
};

#endif

