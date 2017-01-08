// Manager.h

#ifndef _MANAGER_h
#define _MANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <RTClib.h> // for DateTime classes

#include "IServer.h"
#include "IRTC.h"
#include "ClientManager.h"
#include "SensorManager.h"
#include "TechnicalManager.h"

// This main class is a singleton
// http://stackoverflow.com/questions/8811988/can-i-return-a-reference-to-a-static-class-singleton-instance-within-that-clas
// http://stackoverflow.com/questions/13047526/difference-between-singleton-implemention-using-pointer-and-using-static-object
class Manager : public IServer
{
public:
	static Manager& getInstance() {
		static Manager singleton;
		return singleton;
	}

	void setRTC(IRTC * rtc);
	ClientManager & getClientManager() { return mClientManager; }
	SensorManager & getSensorManager() { return mSensorManager; }
	TechnicalManager & getTechnicalManager() { return mTechnicalManager; }

	void work();

private:
	// protect constructors to prevent singleton modification
	Manager();
	Manager(const Manager &rhs);
	Manager & operator=(const Manager &rhs);

	// client notification
	void onClientConnectionChange(bool isConnected);
	// incoming packets
	bool receiveTime(const PacketTime& packet);
	bool receiveSeatOperation(const PacketSeatOperation& packet);

	// sending packets
	const int PACKET_SEND_REPEATS = 3; // after connecting, it will take the client a second or two to perform service discovery and subscribe to notifications. During that time it can't receive packets from the device. We will repeat relevant packets to resolve this.
	bool sendTime();
	bool sendLogicalData();
	bool sendLogicalDataOnce();
	int mRepeatSendLogicalData;
	bool sendCalibrationParams();
	bool sendCalibrationParamsOnce();
	int mRepeatSendCalibrationParams;

	// sub/helper classes
	IRTC * mRTC;
	int16_t mTimeOffsetFromUtcToLocal; // store so we can do a calculation back to utc while the rtc keeps local time. ISSUE: think if this needs to be persistent.
	ClientManager mClientManager;
	SensorManager mSensorManager;
	TechnicalManager mTechnicalManager;

	// helper function
	void PrintDate(Print & out, const DateTime & d);

	// state
	bool mDoneInitialSending;

	// unique device id generation
	unsigned long mDeviceUniqueId; // a unique id. re-generated per boot. generated at first use (at conection to client) which helps get better entropy.
	unsigned long getDeviceUniqueId(); // returns an id, generates it if needed
};

#endif

