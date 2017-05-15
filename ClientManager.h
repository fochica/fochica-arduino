/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ClientManager.h

#ifndef _CLIENTMANAGER_h
#define _CLIENTMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IClientDevice.h"

typedef uint8_t clientCount_t;

// A class representing a facade to all the clients and client devices we have available
class ClientManager : public IClientDevice
{
public:
	ClientManager();
	ClientManager(clientCount_t deviceCount); // pass the number of devices we can have connected
	~ClientManager();

	clientCount_t getConnectedCount();
	void setDeviceCount(clientCount_t deviceCount); // will reset the list
	clientCount_t getDeviceCount() { return mDeviceCount; }
	bool addDevice(IClientDevice * device);
	void reassignListenRight();

	// IClientDevice implementation
	void begin() {};
	bool isConnected() { return getConnectedCount() > 0; };
	void work();
	void setReceiverCallback(IServer * callback);

	// IClient implementation
	bool sendTime(const PacketTime& packet);
	bool sendTechnicalData(const PacketTechnicalData& packet);
	bool sendSensorData(const PacketSensorData& packet);
	bool sendLogicalData(const PacketLogicalData& packet);
	bool sendCalibrationParams(const PacketCalibrationParams& packet);


private:
	IClientDevice ** mDevices; // pointer to an array of pointers of devices
	bool *mDeviceConnState; //  pointer to aray of connection states (prev known state)
	clientCount_t mDeviceCount;
	clientCount_t mDeviceAddedCount;
};

#endif

