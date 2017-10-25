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

#ifdef __AVR__ // this implementation is AVR specific

#include "SensorFreeRAMAVR.h"

SensorFreeRAMAVR::SensorFreeRAMAVR(const char * name) : ISensor(name, SensorType::RAM)
{
}

// should be around 1K-2K for Uno
sensorVal_t SensorFreeRAMAVR::getValueInt()
{
	// based on https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
	// Possibly AVR specific
	// cross platform issue
	extern int __heap_start, *__brkval; // start and end of the heap area
	int v; // variable on the stack, tells us stack boundary
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

float SensorFreeRAMAVR::getValueFloat()
{
	return getValueInt();
}

int SensorFreeRAMAVR::getSamplingTime()
{
	return 0; // no ADC for this sensor
}

void SensorFreeRAMAVR::dumpSRAMBounds(Stream & s)
{
	extern int __heap_start, *__brkval; // start and end of the heap area
	int v; // variable on the stack, tells us stack boundary
	s.print(F("&__heap_start: "));
	s.println((int)&__heap_start);
	s.print(F("__brkval: "));
	s.println((int)__brkval);
	s.print(F("stack: "));
	s.println((int)&v);

}

void SensorFreeRAMAVR::dumpSRAMContent(Stream &s)
{
	// dump all addresses
	for (byte * p = (byte *)RAMSTART; p < (byte *)RAMEND; p++) {
		//s.write(*p);
		s.print((int)p);
		s.print(F(", "));
		if(*p>' ')
			s.write(*p);
		s.print(F(", "));
		s.println((int)*p);
	}
}

#endif