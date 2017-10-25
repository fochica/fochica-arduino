/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorDigital.h

#ifndef _SENSORDIGITAL_h
#define _SENSORDIGITAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

class SensorDigital : public ISensor
{
public:
	SensorDigital(uint8_t pin, int8_t pinMode=-1); // can optionlly define pin mode (INPUT, INPUT_PULLUP or -1 for keep-as-is)
	void begin();

	sensorVal_t getValueInt(); // return 0 or 1*GAIN
	float getValueFloat(); // return 0 or 1
	int getSamplingTime();

private:
	uint8_t mPin;
	int8_t mPinMode;

	const int GAIN = 1000; // scale response to better use int range
};

#endif

