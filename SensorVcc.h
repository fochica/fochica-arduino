/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorVcc.h

#ifndef _SENSORVCC_h
#define _SENSORVCC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that read the Vcc voltage of the controller
// Uses bandgap voltage reference. Measures the known bandgap value using Vcc as the reference. Calculates Vcc based on how the bandgap is sampled.
class SensorVcc : public ISensor
{
public:
	SensorVcc(const char * name);
	sensorVal_t getValueInt(); // return in mV
	float getValueFloat(); // return in V
	int getSamplingTime();

private:
	// Possibly AVR specific
	// cross platform issue
#ifndef MUX4 // if simpler chips (Arduino Uno), such as 168/328
	const int BANDGAP_REF_PIN = 14; // 1110b
#else // if better chips (Arduino Mega), such as 1280/2560
	const int BANDGAP_REF_PIN = 14+16; // 11110b
#endif
	const float BANDGAP_VOLTAGE = 1.1;
	const int SETTLE_DURATION_US = 400; // lower values would result in readings that are lower than actual Vcc
};


#endif

