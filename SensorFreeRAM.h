/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorFreeRAM.h

#ifndef _SENSORFREERAM_h
#define _SENSORFREERAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that reads the amount of available SRAM
class SensorFreeRAM : public ISensor
{
 public:
	 SensorFreeRAM(const char * name);
	 sensorVal_t getValueInt(); // return bytes
	 float getValueFloat(); // return bytes
	 int getSamplingTime();

	 void dumpSRAMContent(Stream &s);
	 void dumpSRAMBounds(Stream &s);
};

#endif

