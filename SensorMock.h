/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorMock.h

#ifndef _SENSORMOCK_h
#define _SENSORMOCK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

struct SensorMockType {
	enum e {
		Saw, // goes from min to max to min with a linear function (param=step size)
		Sine, // goes from min to max to min with a sine function (param=step size in milli 2*pi radians)
		Uniform, // returns sampes from a uniform distribution between in and max
		Normal // returns normal distribution between min and max
	};
};

class SensorMock : public ISensor
{
public:
	SensorMock(const char * name, SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param);
	void setParams(SensorMockType::e type, sensorVal_t min, sensorVal_t max, sensorVal_t param);
	void resetState();

	sensorVal_t getValueInt(); // return in mV
	float getValueFloat(); // return in V
	int getSamplingTime();

private:
	// params
	SensorMockType::e mType;
	sensorVal_t mMin, mMax, mParam;

	// state
	sensorVal_t mCurValue;
	bool mCurDirUp;

	// const
	static const int SINE_STEPS = 1000; // number of sine steps we take in 2pi rads
};

#endif

