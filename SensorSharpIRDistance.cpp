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

#include "SensorSharpIRDistance.h"

SensorSharpIRDistance::SensorSharpIRDistance(uint8_t analogReadPin) : ISensor(SensorType::IRDistance)
{
	mReadPin = analogReadPin;
}

sensorVal_t SensorSharpIRDistance::getValueInt()
{
	int k = analogRead(mReadPin);
	float v = (float)k*ISensor::DEFAULT_ADC_ANALOG_REFERENCE / 1024; // get voltage
	int mm = 240 / v; // convert to distance (mm) per device spec, const is not really important, mapping depends on reflectiveness of the object. can even return [0-1023] analog reading as-is and let the calibrator tidy this data.
	// consider to clamp the distance per device spec [100mm, 800mm]
	return mm;
}

float SensorSharpIRDistance::getValueFloat()
{
	return getValueInt();
}

int SensorSharpIRDistance::getSamplingTime()
{
	return ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
