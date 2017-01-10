// SensorManager.h

#ifndef _SENSORMANAGER_h
#define _SENSORMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"
#include "IClientDevice.h"
#include "CalibratedSensor.h"

typedef uint8_t seatCount_t;
typedef uint8_t sensorCount_t;

struct SensorLocation {
	enum e { Virtual, UnderSeat, Chest, Above };
};

struct SensorState { // relevant for seat sensors
	enum e { 
		None, // no sensors to form an opinion for aggregated sensor or initial state for a single sensor
		Occupied,
		Empty,
		NotCalibrated,
		// aggregated states
		Stabilizing, // disagreement but still in time window to be resolved.
		SensorConflict, // disagreement past time window
		Count
	};
};

class SensorManager
{
public:
	SensorManager(IClientDevice & client);
	~SensorManager();

	void setSeatCount(seatCount_t seatCount);
	void setSensorCount(sensorCount_t sensorCount);
	bool addSensor(seatCount_t seatId, SensorLocation::e location, ISensor * sensor);
	seatCount_t getSeatCount() { return mSeatCount; }
	sensorCount_t getSensorAddedCount() { return mSensorAddedCount; }
	void calibrate(seatCount_t seatId, SensorState::e state);
	bool sendCalibrationParams();

	void work();
	
private:
	struct SensorData {
		ISensor * sensorRaw; // allocated externally
		CalibratedSensor * sensor; // allocated internally
		seatCount_t seatId;
		SensorLocation::e location;
		SensorState::e lastState;
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

	const unsigned long SENSOR_STABILIZE_TIME = 10 * 1000; // in ms
	const bool SOUND_ON_SENSOR_STATE_CHANGE = false;
	const bool SOUND_ON_SEAT_STATE_CHANGE = true;

	void releaseSensorMembers();
	void releaseSeatMembers();
	SensorState::e calibratedSensorStateToSeatSensorState(CalibratedSensorState::e s);
	CalibratedSensorState::e seatSensorStateToCalibratedSensorState(SensorState::e s);
	void sendCalibrationParams(sensorCount_t sensorId);
};


#endif

