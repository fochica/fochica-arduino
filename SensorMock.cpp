// 
// 
// 

#include "SensorMock.h"

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
		double r = getRandGaussian();
		sensorVal_t d = mMax - mMin;
		r=constrain(r, -2, 2); // crop range to [-2,2], some long tail values will affect the extreme bins, not accurage but good enough for this purpose
		r *= (d /2 /2); // scale distribution (2 to Max and -2 to Min to accomodate some long tail and not only use [-1,1]
		r += (mMin + d / 2); // set mean of distribution
		return r;
	}
	case SensorMockType::Uniform:
		return random(mMin, mMax);
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

// get random gaussian distributed value
// mean=0, sigma=1
double SensorMock::getRandGaussian()
{
	// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
	double u, v, S;

	do {
		// The random() function computes a sequence of pseudo - random integers in the range of 0 to \c RANDOM_MAX
		// Arduino's random actually uses this stdlin random function
		u = (double)random() / RANDOM_MAX;
		v = (double)random() / RANDOM_MAX;
		u = 2.0*u - 1; // shift to [-1,1] range
		v = 2.0*v - 1; // shift to [-1,1] range
		S = u*u + v*v; // r^2
	} while (S >= 1); // perform rejection sampling

	// we will only use z1 here. consider to save z2 for next request...
	return u * sqrt(-2.0 * log(S) / S);
}
