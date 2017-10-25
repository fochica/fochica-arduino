/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ISensor.h

#ifndef _ISENSOR_h
#define _ISENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// put the enum in a scope that a struct creates
// http://stackoverflow.com/questions/482745/namespaces-for-enum-types-best-practices
struct SensorType {
	enum e { Vcc, Voltage, RAM, CapacitiveProximity, OnOff, Aggregative, Mock, IRDistance, FSR, CapacitivePressure };
};

typedef int sensorVal_t; // define with typedef so if we want to change this to another type (i.e. an unsigned int) in the future, then we can.

class ISensor
{
 public:
	ISensor(SensorType::e type);

	virtual void begin() {}

	SensorType::e getType() { return mType; }
	 
	virtual sensorVal_t getValueInt() =0; // read sensor value
	virtual float getValueFloat() = 0; // read sensor value
	virtual int getSamplingTime() = 0; // get the time it takes to read a value (estimate) in microseconds

	const int DEFAULT_ADC_SAMPLING_TIME=100; // microseconds
	const int DEFAULT_DIGITAL_SAMPLING_TIME = 1; // microseconds
	const float DEFAULT_ADC_ANALOG_REFERENCE = 5;
	const int MAX_ADC_VALUE = 1023; // including this value

private:
	SensorType::e mType;
};

#endif

