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
	releaseSensorMembers();
}

void SensorManager::setSeatCount(seatCount_t seatCount)
{
	mSeatCount = seatCount;
	// consider validation of existing data... for example that we are not left with sensors whose seatIds are above seatCount
}

void SensorManager::setSensorCount(sensorCount_t sensorCount)
{
	releaseSensorMembers();
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
	mSensors[mSensorAddedCount].sensorRaw = sensor;
	mSensors[mSensorAddedCount].seatId = seatId;
	mSensors[mSensorAddedCount].location = location;
	mSensors[mSensorAddedCount].sensor = new CalibratedSensor(sensor);
	mSensorAddedCount++;
	return true;
}

void SensorManager::calibrate(seatCount_t seatId, SensorState::e state)
{
	// go over all sensors of seat
	for (sensorCount_t sensorId = 0; sensorId < mSensorAddedCount; sensorId++) {
		SensorData& sensor = mSensors[sensorId];
		if (sensor.seatId == seatId) {
			// calibrate them per state
			sensor.sensor->calibrate(seatSensorStateToCalibratedSensorState(state));
			// issue calibration params packet to client
			if (sensor.sensor->isCalibrated()) {
				sendCalibrationParams(sensorId);
			}
		}
	}
}

bool SensorManager::sendCalibrationParams()
{
	// send calibration data for all sensors that are calibrated
	for (sensorCount_t sensorId = 0; sensorId < mSensorAddedCount; sensorId++) {
		SensorData& sensor = mSensors[sensorId];
		if (sensor.sensor->isCalibrated()) {
			sendCalibrationParams(sensorId);
		}
	}
	return true;
}

void SensorManager::work()
{
	// for each seat
	for (seatCount_t seatId = 0; seatId < mSeatCount; seatId++) {
		int aggregatedValue = -1;
		// and sensor
		for (sensorCount_t sensorId=0; sensorId < mSensorAddedCount; sensorId++) {
			SensorData& sensor = mSensors[sensorId];
			if (sensor.seatId == seatId) { // of current seat
				// read sensor values (raw and the state)
				sensorVal_t sensorRawValue;
				CalibratedSensorState::e sensorState = sensor.sensor->getValue(&sensorRawValue);
				// send individual sensor values
				PacketSensorData packet;
				packet.seatId = seatId;
				packet.sensorId = sensorId;
				packet.location = sensor.location;
				packet.type = sensor.sensorRaw->getType();
				packet.value = sensorRawValue;
				packet.state = calibratedSensorStateToSeatSensorState(sensorState);
				mClient.sendSensorData(packet);

				// aggregate
				// TODO, add disagreement states
				if (aggregatedValue == -1) // first sensor
					aggregatedValue = packet.state;
				else {
					if (aggregatedValue != packet.state) // sensor disagreement
						aggregatedValue = 10; // TODO, change
					// else, sensor agreement, keep aggregatedValue as-is
				}
			}
		}
		if (aggregatedValue == -1) // if no sensors
			aggregatedValue = 11; //TODO, change
		
		// generate aggregated sensor (per seat) and send it
		PacketSensorData packet;
		packet.seatId = seatId;
		packet.sensorId = mSensorAddedCount + seatId; // generate unique ids above the existing ids
		packet.location = SensorLocation::Virtual;
		packet.type = SensorType::Aggregative;
		packet.value = aggregatedValue;
		packet.state = aggregatedValue;
		mClient.sendSensorData(packet);
	}
}

void SensorManager::releaseSensorMembers()
{
	if (mSensors) {
		for (int i = 0; i < mSensorAddedCount; i++)
			delete mSensors[i].sensor; // delete the CalibratedSensor instance that we allocated
		delete[] mSensors;
	}
}

SensorState::e SensorManager::calibratedSensorStateToSeatSensorState(CalibratedSensorState::e s)
{
	return s == CalibratedSensorState::A ? SensorState::Occupied : SensorState::Empty;
}

CalibratedSensorState::e SensorManager::seatSensorStateToCalibratedSensorState(SensorState::e s)
{
	return s == SensorState::Occupied ? CalibratedSensorState::A : CalibratedSensorState::B;
}

void SensorManager::sendCalibrationParams(sensorCount_t sensorId)
{
	SensorData& sensor = mSensors[sensorId];
	PacketCalibrationParams packet;
	packet.seatId = sensor.seatId;
	packet.sensorId = sensorId;
	packet.highIsOccupied = sensor.sensor->getStateAIsHigh(); // A==Occupied, A==high => Occupied==high
	packet.lowPassAlpha = sensor.sensor->getExpMovingAverageAlpha();
	packet.triggerHigh = sensor.sensor->getSchmittThresholdHigh();
	packet.triggerLow = sensor.sensor->getSchmittThresholdLow();
	mClient.sendCalibrationParams(packet);
}
