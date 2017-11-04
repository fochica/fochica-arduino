/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ESP32BLEModuleClient.h

#ifndef _ESP32BLEMODULECLIENT_h
#define _ESP32BLEMODULECLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef ESP32 // this implementation is ESP32 specific

#include "ClientDevice.h"
#include "ESP32BLEModule.h"

// Implements a BLE client using one connection of several possible on the ESP32.
class ESP32BLEModuleClient : public ClientDevice
{
public:
	// sending packets
	bool sendTime(const PacketTime& packet);
	bool sendTechnicalData(const PacketTechnicalData& packet);
	bool sendSensorData(const PacketSensorData& packet);
	bool sendLogicalData(const PacketLogicalData& packet);
	bool sendCalibrationParams(const PacketCalibrationParams& packet);

	// receiving logic
	void work() {};
	bool isCanReceivePackets() { return true; };
	bool isListenLimited() { return false; }; // does this device belong to a class where only one can listen at a time?
	void listen() {};

	// state
	void begin() {};
	bool isConnected() { return mModule.isConnected(mId); };

protected:
	friend class ESP32BLEModule; // the module may create instances of connection slots
	ESP32BLEModuleClient(ESP32BLEModule & m, uint16_t id) : mModule(m), mId(id) {};

	// interface for module to call in
	IServer * getServerCallback() { return mServerCallback; } // expose protected memeber in base class to a friend

private:
	ESP32BLEModule & mModule; // the BLE module
	uint16_t mId; // ID of this client connection in the module
};

#endif // ESP32

#endif

