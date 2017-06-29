/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

// returns true if calibration parameters were calculated, and false if only one state parameters were registered
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
	CalibrationParamsState & curStateParams = mCP.stateParams[state];
	curStateParams.avg = total / SAMPLES_FOR_STATE_CALIBRATION;
	curStateParams.max = maxV;
	curStateParams.min = minV;
	curStateParams.collected = true;

	// derrive cleanup and threshold params between states
	// check that other states are also calibrated or non-sense values might be generated
	// assumes CalibratedSensorState.Count==2
	if (!mCP.stateParams[CalibratedSensorState::A].collected || !mCP.stateParams[CalibratedSensorState::B].collected) // exit if mid data collection process
		return false;

	mCP.stateAIsHigh = mCP.stateParams[CalibratedSensorState::A].avg > mCP.stateParams[CalibratedSensorState::B].avg;
	CalibrationParamsState & highStateParams = mCP.stateAIsHigh ? mCP.stateParams[CalibratedSensorState::A] : mCP.stateParams[CalibratedSensorState::B];
	CalibrationParamsState & lowStateParams = mCP.stateAIsHigh ? mCP.stateParams[CalibratedSensorState::B] : mCP.stateParams[CalibratedSensorState::A];

	CalibrationState::e resultState = CalibrationState::Good; // optimistic

	// check dynamic range and warn if needed
	sensorVal_t range = max(lowStateParams.max, highStateParams.max) - min(lowStateParams.min, highStateParams.min);
	if (range < LIMITED_DYNAMIC_RANGE && DebugStream) {
		DebugStream->println(F("Sensor dynamic range is constrained. Consider introducing gain. Please see if this can be improved or try to calibrate again or manually."));
		resultState = CalibrationState::LimitedDynamicRange;
	}

	// check how states intersect
	if (highStateParams.min > lowStateParams.max) { // no intersection
		sensorVal_t gap = highStateParams.min - lowStateParams.max;
		sensorVal_t offset = gap*NO_INTERSECTION_THRESHOLD_PERCENTILE / 100;
		mCP.schmittThresholdHigh = highStateParams.min - offset;
		mCP.schmittThresholdLow = lowStateParams.max + offset;
		mCP.expMovingAverageAlpha = MAX_EXP_ALPHA; // don't smooth, we don't need to. just use the raw value
	} else {
		if (DebugStream)
			DebugStream->println(F("States intersect. This is not recommended. Please see if this can be improved or try to calibrate again or manually."));
		resultState = CalibrationState::StateIntersection;
		if((highStateParams.min<lowStateParams.min && highStateParams.max>lowStateParams.max) || (highStateParams.min>lowStateParams.min && highStateParams.max<lowStateParams.max)) { // if low contained in high or high contained in low
			if (DebugStream)
				DebugStream->println(F("One state is contained in another state. This is highly not recommended. Please see if this can be improved or try to calibrate again or manually."));
			resultState = CalibrationState::StateContainment;
		}
		sensorVal_t meanDist = highStateParams.avg - lowStateParams.avg;
		sensorVal_t lowSpan = lowStateParams.max - lowStateParams.avg;
		sensorVal_t highSpan = highStateParams.avg - highStateParams.min;
		// derrive filtering parameter to make samples more stable and cause the states to intersect less
		float scaleFactor = (float)meanDist/(lowSpan + highSpan); // how much more narrow we want our signal to spread
		mCP.expMovingAverageAlpha = scaleFactor*scaleFactor*MAX_EXP_ALPHA; // alpha=(spread_scale)^2 , not based on mathematical proof, might need tweaking
		mCP.schmittThresholdHigh = highStateParams.avg - (highSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
		mCP.schmittThresholdLow = lowStateParams.avg + (lowSpan*scaleFactor*INTERSECTION_THRESHOLD_PERCENTILE/100);
	}

	mCP.state = resultState; // success!
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
		stream->print(F("State params\t"));
		stream->print(s);
		stream->print('\t');
		stream->print(mCP.stateParams[s].collected);
		if (mCP.stateParams[s].collected) {
			stream->print('\t');
			stream->print(mCP.stateParams[s].min);
			stream->print('\t');
			stream->print(mCP.stateParams[s].avg);
			stream->print('\t');
			stream->print(mCP.stateParams[s].max);
		}
		stream->println();
	}

	// calibration params
	stream->print(F("Sensor params\t"));
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
		mCP.stateParams[s].collected = false;
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
