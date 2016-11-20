// CalibratedSensor.h

#ifndef _CALIBRATEDSENSOR_h
#define _CALIBRATEDSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

struct CalibratedSensorState {
	enum e {
		A,B, Count
	};
};

// A wrapper over a raw sensor input that implements cleanup and thresholding
// including automated process to derrive parameters for cleanup and thresholding
// Thresholding is between two states, A and B. S/B can be Occupied/empty but can also be other meanings, so we can use this for technical sensors too and not only for seat sensors.
// In the future: persistence of calibration parameters
class CalibratedSensor
{
public:
	CalibratedSensor(ISensor * raw, int expAlpha=0, sensorVal_t thLow=0, sensorVal_t thHigh=0); // must provide a valid raw sensor as source

	// reading of value
	CalibratedSensorState::e getValue();

	// calibration
	void Calibrate(CalibratedSensorState::e state);

	// persistence
	// TODO

private:
	ISensor * mRaw;

	// current state
	CalibratedSensorState::e mCurState;
	sensorVal_t mLastRawValue;

	// calibration result
	// cleanup params
	long mExpMovingAverageAlpha; // "Exponentially Weighted Moving Average" alpha param. in milli units. value of 1000 means: use just the new value. use long for not loosing accuracy at high values.
	// thresholding params
	bool mStateAIsHigh;
	sensorVal_t mSchmittThresholdHigh, mSchmittThresholdLow;

	// calibration process data
	// min, max, avg for each state.

	// consts
	static const CalibratedSensorState::e INITIAL_STATE=CalibratedSensorState::A;
	static const sensorVal_t INITIAL_RAW_VALUE=0;
	static const long MAX_EXP_ALPHA = 1000L;

};

#endif

