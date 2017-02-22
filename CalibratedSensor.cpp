// 
// 
// 

#include "CalibratedSensor.h"
#include "DebugStream.h"
#include "PersistentLog.h"

CalibratedSensor::CalibratedSensor(ISensor * raw, int expAlpha, sensorVal_t thLow, sensorVal_t thHigh, CalibratedSensorState::e initialState, sensorVal_t initialValue)
{
	mRaw = raw;
	mCurState = initialState;
	mLastRawValue = initialValue;

	// these come from calibration
	mCP.expMovingAverageAlpha = expAlpha;
	mCP.stateAIsHigh = false;
	mCP.schmittThresholdHigh = thHigh;
	mCP.schmittThresholdLow = thLow;

	resetCalibrationData();
}

CalibratedSensorState::e CalibratedSensor::getValue(sensorVal_t * raw, sensorVal_t * debugFiltered)
{
	// cleanup raw values with a low pass filter
	sensorVal_t val = mRaw->getValueInt();
	if (raw)
		*raw = val;
	long alpha = mCP.expMovingAverageAlpha; // use long to prevent overflow of alpha*val
	val=alpha*val / MAX_EXP_ALPHA + (MAX_EXP_ALPHA - alpha)*mLastRawValue / MAX_EXP_ALPHA;
	if (debugFiltered)
		*debugFiltered = val;
	mLastRawValue = val;

	// change state if threshold is passed
	if (val >= mCP.schmittThresholdHigh)
		mCurState = mCP.stateAIsHigh ? CalibratedSensorState::A : CalibratedSensorState::B;
	else if (val <= mCP.schmittThresholdLow)
		mCurState = mCP.stateAIsHigh ? CalibratedSensorState::B : CalibratedSensorState::A;

	return mCurState;
}

// returns true is calibration parameters were calculated, and false if only one state parameters were registered
bool CalibratedSensor::calibrate(CalibratedSensorState::e state)
{
	// collect params of this state
	long total=0;
	sensorVal_t warmup= mRaw->getValueInt(), minV=warmup, maxV=warmup;
	for (int i = 0; i < SAMPLES_FOR_STATE_CALIBRATION; i++) {
		sensorVal_t v = mRaw->getValueInt();
		total += v;
		minV = min(minV, v);
		maxV = max(maxV, v);
	}
	// save in state
	mStateAvg[state] = total / SAMPLES_FOR_STATE_CALIBRATION;
	mStateMax[state] = maxV;
	mStateMin[state] = minV;
	mStateDataCollected[state] = true;

	// derrive cleanup and threshold params between states
	// assumes other states are also calibrated or non-sense values might be generated
	if (!mStateDataCollected[CalibratedSensorState::A] || !mStateDataCollected[CalibratedSensorState::B]) // exit if mid data collection process
		return false;

	mCP.stateAIsHigh = mStateAvg[CalibratedSensorState::A] > mStateAvg[CalibratedSensorState::B];
	CalibratedSensorState::e high = mCP.stateAIsHigh ? CalibratedSensorState::A : CalibratedSensorState::B;
	CalibratedSensorState::e low = mCP.stateAIsHigh ? CalibratedSensorState::B : CalibratedSensorState::A;

	CalibrationState::e resultState = CalibrationState::Good; // optimistic

	// check dynamic range and warn if needed
	sensorVal_t range = max(mStateMax[low], mStateMax[high]) - min(mStateMin[low], mStateMin[high]);
	if (range < LIMITED_DYNAMIC_RANGE && DebugStream) {
		DebugStream->println(F("Sensor dynamic range is constrained. Consider introducing gain. Please see if this can be improved or try to calibrate again or manually."));
		resultState = CalibrationState::LimitedDynamicRange;
	}

	// check how states intersect
	if (mStateMin[high] > mStateMax[low]) { // no intersection
		sensorVal_t gap = mStateMin[high] - mStateMax[low];
		sensorVal_t offset = gap*NO_INTERSECTION_THRESHOLD_PERCENTILE / 100;
		mCP.schmittThresholdHigh = mStateMin[high] - offset;
		mCP.schmittThresholdLow = mStateMax[low] + offset;
		mCP.expMovingAverageAlpha = MAX_EXP_ALPHA; // don't smooth, we don't need to. just use the raw value
	} else {
		if (DebugStream)
			DebugStream->println(F("States intersect. This is not recommended. Please see if this can be improved or try to calibrate again or manually."));
		resultState = CalibrationState::StateIntersection;
		if((mStateMin[high]<mStateMin[low] && mStateMax[high]>mStateMax[low]) || (mStateMin[high]>mStateMin[low] && mStateMax[high]<mStateMax[low])) { // if low contained in high or high contained in low
			if (DebugStream)
				DebugStream->println(F("One state is contained in another state. This is highly not recommended. Please see if this can be improved or try to calibrate again or manually."));
			resultState = CalibrationState::StateContainment;
		}
		sensorVal_t meanDist = mStateAvg[high] - mStateAvg[low];
		sensorVal_t lowSpan = mStateMax[low] - mStateAvg[low];
		sensorVal_t highSpan = mStateAvg[high] - mStateMin[high];
		// derrive filtering parameter to make samples more stable and cause the states to intersect less
		float scaleFactor = (float)meanDist/(lowSpan + highSpan); // how much more narrow we want our signal to spread
		mCP.expMovingAverageAlpha = scaleFactor*scaleFactor*MAX_EXP_ALPHA; // alpha=(spread_scale)^2 , not based on mathematical proof, might need tweaking
		mCP.schmittThresholdHigh = mStateAvg[high] - (highSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
		mCP.schmittThresholdLow = mStateAvg[low] + (lowSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
	}

	mCP.state = resultState; // success!

	// log
	if (DebugStream)
		debugCalibrationState(DebugStream);
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file)
			debugCalibrationState(file);
		PersistentLog->close();
	}

	return true;
}

void CalibratedSensor::debugCalibrationState()
{
	debugCalibrationState(DebugStream);
}

void CalibratedSensor::debugCalibrationState(Print * stream)
{
	if (stream == NULL)
		return;

	// calibration process data for each state
	for (int s = 0; s < CalibratedSensorState::Count; s++) {
		stream->print(s);
		stream->print('\t');
		stream->print(mStateDataCollected[s]);
		if (mStateDataCollected[s]) {
			stream->print('\t');
			stream->print(mStateMin[s]);
			stream->print('\t');
			stream->print(mStateAvg[s]);
			stream->print('\t');
			stream->print(mStateMax[s]);
		}
		stream->println();
	}

	// calibration params
	stream->print(mCP.state);
	stream->print('\t');
	stream->print(mCP.stateAIsHigh);
	stream->print('\t');
	stream->print(mCP.schmittThresholdLow);
	stream->print('\t');
	stream->print(mCP.schmittThresholdHigh);
	stream->print('\t');
	stream->print(mCP.expMovingAverageAlpha);
	stream->println();
}

bool CalibratedSensor::isCalibrated()
{
	//return mStateDataCollected[CalibratedSensorState::A] && mStateDataCollected[CalibratedSensorState::B];
	// also support when data is loaded from persistent config
	return mCP.state!=CalibrationState::None;
}

void CalibratedSensor::resetCalibrationData()
{
	mCP.state = CalibrationState::None;
	for (int s = 0; s < CalibratedSensorState::Count; s++) {
		mStateDataCollected[s] = false;
	}
}

void CalibratedSensor::setCalibrationParams(CalibrationParams cp)
{
	mCP = cp;
}

const CalibrationParams & CalibratedSensor::getCalibrationParams()
{
	return mCP;
}
