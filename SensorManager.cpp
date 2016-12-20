// 
// 
// 

#include "SensorManager.h"
#include "DebugStream.h"
#include "SoundManager.h"
#include "PersistentSettings.h"

SensorManager::SensorManager(IClientDevice & client) : mClient(client)
{
	mSeatCount = mSensorCount = mSensorAddedCount = 0;
	mSensors = NULL;
	mSeats = NULL;
}

SensorManager::~SensorManager()
{
	releaseSensorMembers();
	releaseSeatMembers();
}

void SensorManager::releaseSensorMembers()
{
	if (mSensors) {
		for (int i = 0; i < mSensorAddedCount; i++)
			delete mSensors[i].sensor; // delete the CalibratedSensor instance that we allocated
		delete[] mSensors;
	}
}

void SensorManager::releaseSeatMembers()
{
	if (mSeats) {
		delete[] mSeats;
	}
}

void SensorManager::setSeatCount(seatCount_t seatCount)
{
	releaseSeatMembers();
	mSeatCount = seatCount;
	mSeats = new SeatData[seatCount]();
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
	// read calibration params
	CalibrationParams cp;
	bool loaded=PersistentSettings::getInstance().readSeatSensorCalibrationParams(mSensorAddedCount, seatId, sensor->getType(), location, cp);
	if (loaded) {
		mSensors[mSensorAddedCount].sensor->setCalibrationParams(cp);
	}

	mSensorAddedCount++;
	return true;
}

void SensorManager::calibrate(seatCount_t seatId, SensorState::e state)
{
	SoundManager::getInstance().playBeep(BeepType::SeatCalibrationStart);
	// go over all sensors of seat
	for (sensorCount_t sensorId = 0; sensorId < mSensorAddedCount; sensorId++) {
		SensorData& sensor = mSensors[sensorId];
		if (sensor.seatId == seatId) {
			// calibrate them per state
			sensor.sensor->calibrate(seatSensorStateToCalibratedSensorState(state));
			// issue calibration params packet to client
			if (sensor.sensor->isCalibrated()) {
				// save calibration params
				PersistentSettings::getInstance().writeSeatSensorCalibrationParams(sensorId, seatId, sensor.sensorRaw->getType(), sensor.location, sensor.sensor->getCalibrationParams());
				// annouce
				sendCalibrationParams(sensorId);
			}
		}
	}
	SoundManager::getInstance().playBeep(BeepType::SeatCalibrationEnd);
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
		SensorState::e aggregatedState = SensorState::None;
		// and sensor
		for (sensorCount_t sensorId=0; sensorId < mSensorAddedCount; sensorId++) {
			SensorData& sensor = mSensors[sensorId];
			if (sensor.seatId == seatId) { // of current seat
				// read sensor values (raw and the state)
				sensorVal_t sensorRawValue;
				CalibratedSensorState::e sensorState = sensor.sensor->getValue(&sensorRawValue);
				SensorState::e state;
				if (sensor.sensor->isCalibrated())
					state = calibratedSensorStateToSeatSensorState(sensorState);
				else
					state = SensorState::NotCalibrated;
				// send individual sensor values
				PacketSensorData packet;
				packet.seatId = seatId;
				packet.sensorId = sensorId;
				packet.location = sensor.location;
				packet.type = sensor.sensorRaw->getType();
				packet.value = sensorRawValue;
				packet.state = state;
				mClient.sendSensorData(packet);

				// aggregate
				if (aggregatedState == SensorState::None) // first sensor
					aggregatedState = state;
				else if (aggregatedState == SensorState::NotCalibrated) {
					// don't care of current sensor state, keep aggregated as NotCalibrated if there is at least one NotCalibrated sensor on seat
				}
				else {
					if (aggregatedState != state) // sensor disagreement
						aggregatedState = SensorState::SensorConflict; // mark as conflict
					// else, sensor agreement, keep aggregatedState as-is
				}

				// respond to state change
				if (sensor.lastState != state && sensor.lastState!= SensorState::None) {
					if (SOUND_ON_SENSOR_STATE_CHANGE) {
						SoundManager::getInstance().playBeep(BeepType::SensorStateChange);
					}
				}
				sensor.lastState = state; // save
			}
		}

		// decide on state
		if (aggregatedState != SensorState::None) // if not "no sensors"
		{
			if (aggregatedState == SensorState::SensorConflict) {
				unsigned long timeFromLastAgreement = millis() - mSeats[seatId].lastSensorAgreementTime;
				if (timeFromLastAgreement < SENSOR_STABILIZE_TIME)
					aggregatedState = SensorState::Stabilizing;
			}
			else { // all sensor in agreement about some value
				mSeats[seatId].lastSensorAgreementTime = millis();
			}
		}

		if (mSeats[seatId].lastState != aggregatedState && mSeats[seatId].lastState!= SensorState::None) { // if change in state
			if (mClient.isConnected()) { // have connections to client
				if (SOUND_ON_SEAT_STATE_CHANGE) {
					SoundManager::getInstance().playBeep(BeepType::SeatStateChange);
				}
			}
			else { // no connection to client, alert
				SoundManager::getInstance().playBeep(BeepType::SeatStateChangeNoClient);
			}
		}
		mSeats[seatId].lastState = aggregatedState; // save state for change detection next time
		
		// generate aggregated sensor (per seat) and send it
		PacketSensorData packet;
		packet.seatId = seatId;
		packet.sensorId = mSensorAddedCount + seatId; // generate unique ids above the existing ids
		packet.location = SensorLocation::Virtual;
		packet.type = SensorType::Aggregative;
		packet.value = aggregatedState;
		packet.state = aggregatedState;
		mClient.sendSensorData(packet);
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
