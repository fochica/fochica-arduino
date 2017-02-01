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
#include "IEventHandlerState.h"
#include "IRTC.h"
#include "ClientManager.h"
#include "SensorManager.h"
#include "TechnicalManager.h"
#include "IEventHandler.h"

// This main class is a singleton
// http://stackoverflow.com/questions/8811988/can-i-return-a-reference-to-a-static-class-singleton-instance-within-that-clas
// http://stackoverflow.com/questions/13047526/difference-between-singleton-implemention-using-pointer-and-using-static-object
class Manager : public IServer, IEventHandlerState, ISensorManagerCallback
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

	// IEventHandlerState (functionality provided for EventHandlers to get state)
	bool isConnected() { mClientManager.isConnected(); }
	seatCount_t getSeatCount() { mSensorManager.getSeatCount(); }
	SensorState::e getSeatState(seatCount_t seatId) { return mSensorManager.getSeatState(seatId); }

	// event handlers list
	void setEventHandlerCount(int count);
	bool addEventHandler(IEventHandler * handler);

	// sensor manager callback
	bool eventSeatStateChange(seatCount_t seatId, SensorState::e lastState, SensorState::e newState);

private:
	// protect constructors to prevent singleton modification
	Manager();
	Manager(const Manager &rhs);
	Manager & operator=(const Manager &rhs);
	~Manager();

	// client notification
	void onClientConnectionChange(bool isConnected);
	// incoming packets
	bool receiveTime(const PacketTime& packet);
	bool receiveSeatOperation(const PacketSeatOperation& packet);
	bool receiveSensorOperation(const PacketSensorOperation& packet);

	// sending packets
	bool sendTime();
	bool sendLogicalData();
	bool sendCalibrationParams();
	// sending repeat logic
	// after connecting, it will take the client a second or two to perform service discovery and to subscribe to notifications. During that time it can't receive packets from the device. We will repeat sending of relevant packets to resolve this.
	// this might not be relevant to all modules/transports. Consider to move this implementation to other classes of the projects if we ever need to add transport other than BLE.
	const int PACKET_SEND_REPEATS = 3;
	bool sendLogicalDataOnce();
	int mRepeatSendLogicalData;
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

	// event handlers
	IEventHandler ** mEventHandlers; // pointer to an array of event handlers. this class doesn't allocate or free the handlers, just the array.
	int mEventHandlerCount;
	int mEventHandlerAddedCount;
	void releaseEventHandlers();
	// distribution helpers
	void eventHandlersWork();
	bool eventClientConnectionChange(bool isAdapterConnected);

};

#endif

