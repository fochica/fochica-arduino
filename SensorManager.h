// SensorManager.h

#ifndef _SENSORMANAGER_h
#define _SENSORMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

typedef uint8_t seatCount_t;
typedef uint8_t sensorCount_t;

class SensorManager
{
public:
	SensorManager();
	~SensorManager();

	void setSeatCount(seatCount_t seatCount);
	void setSensorCount(sensorCount_t sensorCount);
	bool addSensor(seatCount_t seatId, ISensor * sensor);

	void work();
	
private:
	seatCount_t mSeatCount;
	sensorCount_t mSensorCount;
	sensorCount_t mSensorAddedCount;
	ISensor ** mSensors;

};


#endif

