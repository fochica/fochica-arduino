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
	CalibratedSensorState::e getValue(sensorVal_t * raw=NULL, sensorVal_t * debugFiltered=NULL);

	// calibration
	void calibrate(CalibratedSensorState::e state);
	void debugCalibrationState();
	bool isCalibrated();
	void resetCalibrationData();

	long getExpMovingAverageAlpha() {
		return mExpMovingAverageAlpha;
	}
	bool getStateAIsHigh() {
		return mStateAIsHigh;
	}
	sensorVal_t getSchmittThresholdHigh() {
		return mSchmittThresholdHigh;
	}
	sensorVal_t getSchmittThresholdLow() {
		return mSchmittThresholdLow;
	}

	// persistence
	// TODO

	// consts
	static const long MAX_EXP_ALPHA = 1000L;

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
	sensorVal_t mStateMin[CalibratedSensorState::Count];
	sensorVal_t mStateMax[CalibratedSensorState::Count];
	sensorVal_t mStateAvg[CalibratedSensorState::Count];
	bool mStateDataCollected[CalibratedSensorState::Count];

	// consts
	static const CalibratedSensorState::e INITIAL_STATE = CalibratedSensorState::A; // the initial state of this sensor
	static const sensorVal_t INITIAL_RAW_VALUE = 0; // the value to initialize our low pass filter with
	static const int SAMPLES_FOR_STATE_CALIBRATION = 100; // how many samples to take to determine properties of a certain sensor state
	static const int NO_INTERSECTION_THRESHOLD_PERCENTILE = 10; // where to place high/low thresholds in the gap (between states) if the states don't intersect
	static const int INTERSECTION_THRESHOLD_PERCENTILE = 50; // where to place high/low thresholds in the span (of each state) if the states do intersect
	static const int LIMITED_DYNAMIC_RANGE = 100; // dynamic range threshold across all states that is not optimal (too low)
};

#endif

