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

struct CalibrationState {
	enum e {
		None, Good, LimitedDynamicRange, StateIntersection, StateContainment
	};
};

// part of the persistent schema, probably need to increase version number if this is changed
struct CalibrationParams {
	CalibrationState::e state;
	// cleanup params
	int expMovingAverageAlpha; // "Exponentially Weighted Moving Average" alpha param. in milli units. value of 1000 means: use just the new value.
	// thresholding params
	bool stateAIsHigh;
	sensorVal_t schmittThresholdHigh, schmittThresholdLow;
};

// A wrapper over a raw sensor input that implements cleanup and thresholding
// including automated process to derrive parameters for cleanup and thresholding
// Thresholding is between two states, A and B. S/B can be Occupied/empty but can also be other meanings, so we can use this for technical sensors too and not only for seat sensors.
// In the future: persistence of calibration parameters
class CalibratedSensor
{
public:
	CalibratedSensor(ISensor * raw, int expAlpha=0, sensorVal_t thLow=0, sensorVal_t thHigh=0, CalibratedSensorState::e initialState= INITIAL_STATE, sensorVal_t initialValue=INITIAL_RAW_VALUE); // must provide a valid raw sensor as source

	// reading of value
	CalibratedSensorState::e getValue(sensorVal_t * raw=NULL, sensorVal_t * debugFiltered=NULL);

	// calibration
	void calibrate(CalibratedSensorState::e state);
	void debugCalibrationState();
	bool isCalibrated();
	CalibrationState::e getCalibrationState() { return mCP.state; }
	void resetCalibrationData();

	int getExpMovingAverageAlpha() {
		return mCP.expMovingAverageAlpha;
	}
	bool getStateAIsHigh() {
		return mCP.stateAIsHigh;
	}
	sensorVal_t getSchmittThresholdHigh() {
		return mCP.schmittThresholdHigh;
	}
	sensorVal_t getSchmittThresholdLow() {
		return mCP.schmittThresholdLow;
	}

	// persistence
	void setCalibrationParams(CalibrationParams cp);
	const CalibrationParams & getCalibrationParams();

	// consts
	static const int MAX_EXP_ALPHA = 1000;

private:
	ISensor * mRaw;

	// current state
	CalibratedSensorState::e mCurState;
	sensorVal_t mLastRawValue;

	// calibration result
	CalibrationParams mCP;

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

