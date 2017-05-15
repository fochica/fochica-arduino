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

#include "ClientManager.h"
#include "DebugStream.h"

ClientManager::ClientManager()
{
	mDeviceCount = 0;
	mDevices = NULL;
	mDeviceAddedCount = 0;
	mServerCallback = NULL;
}

ClientManager::ClientManager(clientCount_t deviceCount) : ClientManager()
{
	setDeviceCount(deviceCount);
}

ClientManager::~ClientManager()
{
	if (mDevices)
		delete[] mDevices;
}

void ClientManager::setDeviceCount(clientCount_t deviceCount)
{
	// release prev data, if exists
	if (mDevices)
		delete[] mDevices;
	// allocate and update state
	mDeviceCount = deviceCount;
	mDevices = new IClientDevice*[deviceCount]();
	mDeviceAddedCount = 0;
}

clientCount_t ClientManager::getConnectedCount()
{
	clientCount_t count = 0;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++)
		if (mDevices[i]->isConnected())
			count++;
	return count;
}

bool ClientManager::addDevice(IClientDevice * device)
{
	if (mDeviceAddedCount >= mDeviceCount) {
		if(DebugStream!=NULL) DebugStream->println(F("ClientManager::addDevice, trying to add more devices than declared"));
		return false;
	}
	mDevices[mDeviceAddedCount++] = device;
	device->setReceiverCallback(mServerCallback); // note, we don't clear callback from devices on destruction, assume we initialize only once
	return true;
}

void ClientManager::reassignListenRight()
{
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isListenLimited() && mDevices[i]->isConnected()) { // find a device that is listen-limited and connected
			mDevices[i]->listen(); // make it the one that listens
			break;
		}
	}
}

void ClientManager::setReceiverCallback(IServer * callback)
{
	mServerCallback = callback;
	// apply to all already added devices
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++)
		mDevices[i]->setReceiverCallback(callback);
}

void ClientManager::work()
{
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		mDevices[i]->work();
	}
}

// IClient implementation
bool ClientManager::sendTime(const PacketTime & packet)
{
	bool ret = true;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected()) // only send data if connected, otherwise we will send commands (in command mode). Should this logic be here or in the device implementation?
			ret &= mDevices[i]->sendTime(packet);
	}
	return ret;
}

bool ClientManager::sendTechnicalData(const PacketTechnicalData & packet)
{
	bool ret = true;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendTechnicalData(packet);
	}
	return ret;
}

bool ClientManager::sendSensorData(const PacketSensorData & packet)
{
	bool ret = true;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendSensorData(packet);
	}
	return ret;
}

bool ClientManager::sendLogicalData(const PacketLogicalData & packet)
{
	bool ret = true;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected()) {
			// we need to add device specific capabilities to this packet type, so make a copy of the const
			PacketLogicalData specificPacket(packet);
			// check capabilities
			if(mDevices[i]->isCanReceivePackets())
				specificPacket.capabilityMask |= Capabilities::CanReceivePackets;
			if (DebugStream) {
				DebugStream->print(F("Adapter capability: "));
				DebugStream->println(specificPacket.capabilityMask);
			}
			// send
			ret &= mDevices[i]->sendLogicalData(specificPacket);
		}
	}
	return ret;
}

bool ClientManager::sendCalibrationParams(const PacketCalibrationParams & packet)
{
	bool ret = true;
	for (clientCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendCalibrationParams(packet);
	}
	return ret;
}
