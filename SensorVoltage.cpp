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

#include "SensorVoltage.h"

SensorVoltage::SensorVoltage(uint8_t analogPin, long resistorToGround, long resistorToVoltage, float aReference) : ISensor(SensorType::Voltage)
{
	mPin = analogPin;
	mDivider = (resistorToGround + resistorToVoltage) / resistorToGround;
	mAReference = aReference;
}

// returns value in mV. don't use as-it-is now for measuring voltage over 32V
sensorVal_t SensorVoltage::getValueInt()
{
	return getValueFloat() * 1000;
}

float SensorVoltage::getValueFloat()
{
	int v = analogRead(mPin);
	return (float)mDivider*mAReference*v / ISensor::MAX_ADC_VALUE;
}

int SensorVoltage::getSamplingTime()
{
	return ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
