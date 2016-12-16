// ClientManager.h

#ifndef _CLIENTMANAGER_h
#define _CLIENTMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
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

