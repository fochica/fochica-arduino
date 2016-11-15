// 
// 
// 

#include "SensorManager.h"

SensorManager::SensorManager()
{
	mSeatCount = mSensorCount = mSensorAddedCount = 0;
	mSensors = NULL;
}

SensorManager::~SensorManager()
{
	if (mSensors)
		delete[] mSensors;
}

void SensorManager::setSeatCount(seatCount_t seatCount)
{
	mSeatCount = seatCount;
	// consider validation of existing data...
}

void SensorManager::setSensorCount(sensorCount_t sensorCount)
{
	if (mSensors)
		delete[] mSensors;
	mSensors = new ISensor *[sensorCount];
	mSensorCount = sensorCount;
	mSensorAddedCount = 0;
}

bool SensorManager::addSensor(seatCount_t seatId, ISensor * sensor)
{
	// TODO
	return false;
}

void SensorManager::work()
{
	// TODO
}
