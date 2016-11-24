// 
// 
// 

#include "CalibratedSensor.h"
#include "DebugStream.h"

CalibratedSensor::CalibratedSensor(ISensor * raw, int expAlpha, sensorVal_t thLow, sensorVal_t thHigh)
{
	mRaw = raw;
	mCurState = INITIAL_STATE;
	mLastRawValue = INITIAL_RAW_VALUE;

	// these come from calibration
	mExpMovingAverageAlpha = expAlpha;
	mStateAIsHigh = false;
	mSchmittThresholdHigh = thHigh;
	mSchmittThresholdLow = thLow;

	resetCalibrationData();
	// TODO, load previous persistent state
}

CalibratedSensorState::e CalibratedSensor::getValue(sensorVal_t * raw, sensorVal_t * debugFiltered)
{
	// cleanup raw values with a low pass filter
	sensorVal_t val = mRaw->getValueInt();
	if (raw)
		*raw = val;
	val=mExpMovingAverageAlpha*val / MAX_EXP_ALPHA + (MAX_EXP_ALPHA - mExpMovingAverageAlpha)*mLastRawValue / MAX_EXP_ALPHA;
	if (debugFiltered)
		*debugFiltered = val;
	mLastRawValue = val;

	// change state if threshold is passed
	if (val >= mSchmittThresholdHigh)
		mCurState = mStateAIsHigh ? CalibratedSensorState::A : CalibratedSensorState::B;
	else if (val <= mSchmittThresholdLow)
		mCurState = mStateAIsHigh ? CalibratedSensorState::B : CalibratedSensorState::A;

	return mCurState;
}

void CalibratedSensor::calibrate(CalibratedSensorState::e state)
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
		return;

	mStateAIsHigh = mStateAvg[CalibratedSensorState::A] > mStateAvg[CalibratedSensorState::B];
	CalibratedSensorState::e high = mStateAIsHigh ? CalibratedSensorState::A : CalibratedSensorState::B;
	CalibratedSensorState::e low = mStateAIsHigh ? CalibratedSensorState::B : CalibratedSensorState::A;

	// check dynamic range and warn if needed
	sensorVal_t range = max(mStateMax[low], mStateMax[high]) - min(mStateMin[low], mStateMin[high]);
	if (range < LIMITED_DYNAMIC_RANGE && DebugStream)
		DebugStream->println(F("Sensor dynamic range is constrained. Consider introducing gain. Please see if this can be improved or try to calibrate again or manually."));

	// check how states intersect
	if (mStateMin[high] > mStateMax[low]) { // no intersection
		sensorVal_t gap = mStateMin[high] - mStateMax[low];
		sensorVal_t offset = gap*NO_INTERSECTION_THRESHOLD_PERCENTILE / 100;
		mSchmittThresholdHigh = mStateMin[high] - offset;
		mSchmittThresholdLow = mStateMax[low] + offset;
		mExpMovingAverageAlpha = MAX_EXP_ALPHA; // don't smooth, we don't need to. just use the raw value
	} else {
		if (DebugStream) DebugStream->println(F("States intersect. This is not recommended. Please see if this can be improved or try to calibrate again or manually."));
		if((mStateMin[high]<mStateMin[low] && mStateMax[high]>mStateMax[low]) || (mStateMin[high]>mStateMin[low] && mStateMax[high]<mStateMax[low])) // if low contained in high or high contained in low
			if (DebugStream) DebugStream->println(F("One state is contained in another state. This is highly not recommended. Please see if this can be improved or try to calibrate again or manually."));
		sensorVal_t meanDist = mStateAvg[high] - mStateAvg[low];
		sensorVal_t lowSpan = mStateMax[low] - mStateAvg[low];
		sensorVal_t highSpan = mStateAvg[high] - mStateMin[high];
		// derrive filtering parameter to make samples more stable and cause the states to intersect less
		float scaleFactor = (float)meanDist/(lowSpan + highSpan); // how much more narrow we want our signal to spread
		mExpMovingAverageAlpha = scaleFactor*scaleFactor*MAX_EXP_ALPHA; // alpha=(spread_scale)^2 , not based on mathematical proof, might need tweaking
		mSchmittThresholdHigh = mStateAvg[high] - (highSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
		mSchmittThresholdLow = mStateAvg[low] + (lowSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
	}
}

void CalibratedSensor::debugCalibrationState()
{
	if (DebugStream == NULL)
		return;

	// calibration process data for each state
	for (int s = 0; s < CalibratedSensorState::Count; s++) {
		DebugStream->print(s);
		DebugStream->print('\t');
		DebugStream->print(mStateMin[s]);
		DebugStream->print('\t');
		DebugStream->print(mStateAvg[s]);
		DebugStream->print('\t');
		DebugStream->print(mStateMax[s]);
		DebugStream->print('\t');
		DebugStream->print(mStateDataCollected[s]);
		DebugStream->println();
	}

	// calibration params
	DebugStream->print(mStateAIsHigh);
	DebugStream->print('\t');
	DebugStream->print(mSchmittThresholdLow);
	DebugStream->print('\t');
	DebugStream->print(mSchmittThresholdHigh);
	DebugStream->print('\t');
	DebugStream->print(mExpMovingAverageAlpha);
	DebugStream->println();
}

bool CalibratedSensor::isCalibrated()
{
	return mStateDataCollected[CalibratedSensorState::A] && mStateDataCollected[CalibratedSensorState::B];
}

void CalibratedSensor::resetCalibrationData()
{
	for (int s = 0; s < CalibratedSensorState::Count; s++) {
		mStateDataCollected[s] = false;
	}
}
