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
	mCallback = NULL;
}

ClientManager::ClientManager(deviceCount_t deviceCount) : ClientManager()
{
	setDeviceCount(deviceCount);
}

ClientManager::~ClientManager()
{
	if (mDevices)
		delete[] mDevices;
}

void ClientManager::setDeviceCount(deviceCount_t deviceCount)
{
	// release prev data, if exists
	if (mDevices)
		delete[] mDevices;
	// allocate and update state
	mDeviceCount = deviceCount;
	mDevices = new IClientDevice*[deviceCount]();
	mDeviceAddedCount = 0;
}

deviceCount_t ClientManager::getConnectedCount()
{
	deviceCount_t count = 0;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++)
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
	device->setReceiverCallback(mCallback); // note, we don't clear callback from devices on destruction, assume we initialize only once
	return true;
}

void ClientManager::setReceiverCallback(IServer * callback)
{
	mCallback = callback;
	// apply to all already added devices
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++)
		mDevices[i]->setReceiverCallback(callback);
}

void ClientManager::work()
{
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		mDevices[i]->work();
	}
}

// IClient implementation
bool ClientManager::sendTime(const PacketTime & packet)
{
	bool ret = true;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected()) // only send data if connected, otherwise we will send commands (in command mode). Should this logic be here or in the device implementation?
			ret &= mDevices[i]->sendTime(packet);
	}
	return ret;
}

bool ClientManager::sendTechnicalData(const PacketTechnicalData & packet)
{
	bool ret = true;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendTechnicalData(packet);
	}
	return ret;
}

bool ClientManager::sendSensorData(const PacketSensorData & packet)
{
	bool ret = true;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendSensorData(packet);
	}
	return ret;
}

bool ClientManager::sendLogicalData(const PacketLogicalData & packet)
{
	bool ret = true;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendLogicalData(packet);
	}
	return ret;
}

bool ClientManager::sendCalibrationParams(const PacketCalibrationParams & packet)
{
	bool ret = true;
	for (deviceCount_t i = 0; i < mDeviceAddedCount; i++) {
		if (mDevices[i]->isConnected())
			ret &= mDevices[i]->sendCalibrationParams(packet);
	}
	return ret;
}
