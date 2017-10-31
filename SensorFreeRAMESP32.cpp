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

#include "SensorFreeRAMESP32.h"

#ifdef ESP32 // this implementation is ESP32 specific

SensorFreeRAMESP32::SensorFreeRAMESP32() : ISensor(SensorType::RAM)
{
}

// should be around 1K-2K for Uno
sensorVal_t SensorFreeRAMESP32::getValueInt()
{
	return esp_get_free_heap_size();
}

float SensorFreeRAMESP32::getValueFloat()
{
	return getValueInt();
}

int SensorFreeRAMESP32::getSamplingTime()
{
	return 0; // no ADC time for this sensor
}


#endif