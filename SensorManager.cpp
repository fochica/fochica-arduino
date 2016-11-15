// 
// 
// 

#include "SensorManager.h"
#include "DebugStream.h"

SensorManager::SensorManager(IClient & client) : mClient(client)
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
	mSensors = new SensorData[sensorCount];
	mSensorCount = sensorCount;
	mSensorAddedCount = 0;
}

bool SensorManager::addSensor(seatCount_t seatId, SensorLocation::e location, ISensor * sensor)
{
	if (mSensorAddedCount >= mSensorCount) {
		if (DebugStream != NULL) DebugStream->println(F("SensorManager::addSensor, trying to add more sensors than declared"));
		return false;
	}
	mSensors[mSensorAddedCount].sensor = sensor;
	mSensors[mSensorAddedCount].seatId = seatId;
	mSensors[mSensorAddedCount].location = location;
	mSensorAddedCount++;
	return true;
}

void SensorManager::work()
{
	// for each seat
	for (seatCount_t seatId = 0; seatId <= mSeatCount; seatId++) {
		int aggregatedValue = 0;
		// and sensor
		for (sensorCount_t sensorId; sensorId <= mSensorAddedCount; sensorId++) {
			SensorData& sensor = mSensors[sensorId];
			if (sensor.seatId == seatId) { // of current seat
				// read calibrated sensor values (what does this mean?)
				int sensorValue=sensor.sensor->getValueInt();
				// send individual sensor values
				PacketSensorData packet;
				packet.seatId = seatId;
				packet.sensorId = sensorId;
				packet.location = sensor.location;
				packet.type = sensor.sensor->getType();
				packet.value = sensorValue;
				mClient.sendSensorData(packet);

				// aggregate
				// TODO
				if (sensorValue > 0)
					aggregatedValue++;
			}
		}

		// generate aggregated sensor (for seat) and send it
		PacketSensorData packet;
		packet.seatId = seatId;
		packet.sensorId = mSensorCount+seatId; // generate unique ids above the existing ids
		packet.location = SensorLocation::Virtual;
		packet.type = SensorType::Aggregative;
		packet.value = aggregatedValue;
		mClient.sendSensorData(packet);
	}
}
