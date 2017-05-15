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

#include "SensorMock.h"
#include "RNGUtils.h"

SensorMock::SensorMock(const char * name, SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param) : ISensor(name, SensorType::Mock)
{
	setParams(type, min, max, param);
}

void SensorMock::setParams(SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param)
{
	mType = type;
	mMin = min;
	mMax = max;
	mParam = param;

	resetState();
}

void SensorMock::resetState()
{
	mCurValue = mMin;
	mCurDirUp = true;
}

sensorVal_t SensorMock::getValueInt()
{
	switch (mType) {
	case SensorMockType::Normal:
	{
		double r = RNGUtils::getGaussian();
		sensorVal_t d = mMax - mMin;
		r=constrain(r, -2, 2); // crop range to [-2,2], some long tail values will affect the extreme bins, not accurage but good enough for this purpose
		r *= (d /2 /2); // scale distribution (2 to Max and -2 to Min to accomodate some long tail and not only use [-1,1]
		r += (mMin + d / 2); // set mean of distribution
		return r;
	}
	case SensorMockType::Uniform:
		return RNGUtils::getLong(mMin, mMax);
	case SensorMockType::Saw:
		if (mCurDirUp) {
			mCurValue += mParam;
			if (mCurValue + mParam > mMax)
				mCurDirUp = false; // reverse direction
		}
		else { // going down
			mCurValue -= mParam;
			if (mCurValue - mParam < mMin)
				mCurDirUp = true; // reverse direction
		}
		return mCurValue;
	case SensorMockType::Sine:
	{
		// get sine value
		mCurValue += mParam;
		double s = sin(PI * 2 * mCurValue / SINE_STEPS);
		// handle cycle
		if (mCurValue + mParam > SINE_STEPS)
			mCurValue = 0; // considered substracting SINE_STEPS, but it may cause a negative value which might not be supported by signalVal type.
		// map to our range
		sensorVal_t d = mMax - mMin;
		s *= (d / 2); // scale distribution
		s += (mMin + d / 2); // set mean of distribution
		return s;
	}
	default:
		return 0;
	}
}

float SensorMock::getValueFloat()
{
	return getValueInt();
}

int SensorMock::getSamplingTime()
{
	return 0;
}
