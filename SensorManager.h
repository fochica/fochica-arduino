// SensorManager.h

#ifndef _SENSORMANAGER_h
#define _SENSORMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"
#include "IClient.h"

typedef uint8_t seatCount_t;
typedef uint8_t sensorCount_t;

struct SensorLocation {
	enum e { Virtual, UnderSeat, Chest, Above };
};

class SensorManager
{
public:
	SensorManager(IClient & client);
	~SensorManager();

	void setSeatCount(seatCount_t seatCount);
	void setSensorCount(sensorCount_t sensorCount);
	bool addSensor(seatCount_t seatId, SensorLocation::e location, ISensor * sensor);

	void work();
	
private:
	struct SensorData {
		ISensor * sensor;
		seatCount_t seatId;
		SensorLocation::e location;
	};

	seatCount_t mSeatCount;
	sensorCount_t mSensorCount;
	sensorCount_t mSensorAddedCount;
	SensorData* mSensors;
	//seatCount_t * mSeatOfSensor; // seat id for each sensor

	IClient & mClient; // IClient interface to client manager, encapsulating several clients and client devices

};


#endif

