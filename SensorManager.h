/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorManager.h

#ifndef _SENSORMANAGER_h
#define _SENSORMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"
#include "IClientDevice.h"
#include "CalibratedSensor.h"
#include "ISensorManagerCallback.h"
#include "SensorMisc.h"

// part of the persistent schema, probably need to increase version number if this is changed
struct SensorPersistentParams {
	SensorPersistentParams() {};
	SensorPersistentParams(const CalibrationParams & cp, SensorActivityMode::e am) :calibrationParams(cp), activityMode(am) {
	}

	CalibrationParams calibrationParams;
	SensorActivityMode::e activityMode;
};

class SensorManager
{
public:
	SensorManager(IClientDevice & client, ISensorManagerCallback & callback);
	~SensorManager();

	void setSeatCount(seatCount_t seatCount);
	void setSensorCount(sensorCount_t sensorCount);
	bool addSensor(seatCount_t seatId, SensorLocation::e location, ISensor * sensor);
	seatCount_t getSeatCount() { return mSeatCount; }
	sensorCount_t getSensorAddedCount() { return mSensorAddedCount; }
	void calibrate(seatCount_t seatId, SensorState::e state);
	bool sendCalibrationParams();
	void setSensorActivityMode(sensorCount_t sensorId, SensorActivityMode::e activityMode);
	SensorState::e getSeatState(seatCount_t seatId);

	void work();
	
private:
	struct SensorData {
		ISensor * sensorRaw; // allocated externally
		CalibratedSensor * sensor; // allocated internally
		seatCount_t seatId;
		SensorLocation::e location;
		SensorState::e lastState;
		SensorActivityMode::e activityMode;
	};

	struct SeatData {
		unsigned long lastSensorAgreementTime; // in ms using millis()
		SensorState::e lastState;
	};

	seatCount_t mSeatCount;
	sensorCount_t mSensorCount;
	sensorCount_t mSensorAddedCount;
	SensorData* mSensors;
	SeatData* mSeats;

	IClientDevice & mClient; // IClient interface to client manager, encapsulating several clients and client devices
	ISensorManagerCallback & mCallback; // interface back to Manager

	const unsigned long SENSOR_STABILIZE_TIME = 10 * 1000; // in ms

	void releaseSensorMembers();
	void releaseSeatMembers();
	SensorState::e calibratedSensorStateToSeatSensorState(CalibratedSensorState::e s);
	CalibratedSensorState::e seatSensorStateToCalibratedSensorState(SensorState::e s);
	void sendCalibrationParams(sensorCount_t sensorId);
};


#endif

