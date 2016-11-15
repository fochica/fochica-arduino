// ClientManager.h

#ifndef _CLIENTMANAGER_h
#define _CLIENTMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IClientDevice.h"

typedef uint8_t deviceCount_t;

// A class representing a facade to all the clients and client devices we have available
class ClientManager : public IClient
{
public:
	ClientManager();
	ClientManager(deviceCount_t deviceCount); // pass the number of devices we can have connected
	~ClientManager();

	deviceCount_t getConnectedCount();
	void setDeviceCount(deviceCount_t deviceCount); // will reset the list
	deviceCount_t getDeviceCount() { return mDeviceCount; }
	bool addDevice(IClientDevice * device);
	void setReceiverCallback(IServer * callback);

	void work();

	// IClient implementation
	bool sendTime(const PacketTime& packet);
	bool sendTechnicalData(const PacketTechnicalData& packet);
	bool sendSensorData(const PacketSensorData& packet);

private:
	IClientDevice ** mDevices; // pointer to an array of pointers of devices
	bool *mDeviceConnState; //  pointer to aray of connection states (prev known state)
	deviceCount_t mDeviceCount;
	deviceCount_t mDeviceAddedCount;
	IServer * mCallback; // pointer to a callback interface for incoming data
};

#endif

