// 
// 
// 

#include "CalibratedSensor.h"

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

	// TODO, load revious persistent state
}

CalibratedSensorState::e CalibratedSensor::getValue()
{
	// cleanup raw values with a low pass filter
	sensorVal_t val = mRaw->getValueInt();
	val=mExpMovingAverageAlpha*val / MAX_EXP_ALPHA + (MAX_EXP_ALPHA - mExpMovingAverageAlpha)*mLastRawValue / MAX_EXP_ALPHA;
	mLastRawValue = val;

	// change state if threshold is passed
	if (val >= mSchmittThresholdHigh)
		mCurState = mStateAIsHigh ? CalibratedSensorState::A : CalibratedSensorState::B;
	else if (val <= mSchmittThresholdLow)
		mCurState = mStateAIsHigh ? CalibratedSensorState::B : CalibratedSensorState::A;

	return mCurState;
}

void CalibratedSensor::Calibrate(CalibratedSensorState::e state)
{
}
