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

	void work();

private:
	// protect constructors to prevent singleton modification
	Manager() : mSensorManager(mClientManager) { }
	Manager(const Manager &rhs);
	Manager & operator=(const Manager &rhs);

	// incoming packets
	bool receiveTime(const PacketTime& packet);

	// sending packets
	bool sendTime();

	// sub/helper classes
	IRTC * mRTC;
	int16_t mTimeOffsetFromUtcToLocal; // store so we can do a calculation back to utc while the rtc keeps local time. ISSUE: think if this needs to be persistent.
	ClientManager mClientManager;
	SensorManager mSensorManager;

	// helper function
	void PrintDate(Print & out, const DateTime & d);
};

#endif

