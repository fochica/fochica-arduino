/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorSharpIRDistance.h

#ifndef _SENSORSHARPIRDISTANCE_h
#define _SENSORSHARPIRDISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// An analog IR distance sensor by Sharp.
// Such as GP2Y0A21YK0F or similar.
// Assumes analog reference is default and is at value specified by ISensor::DEFAULT_ADC_ANALOG_REFERENCE
class SensorSharpIRDistance : public ISensor
{
public:
	SensorSharpIRDistance(uint8_t analogReadPin);
	sensorVal_t getValueInt(); // returns distance in mm
	float getValueFloat();
	int getSamplingTime();

private:
	uint8_t mReadPin;
	float mAnalogRef;

};

#endif

