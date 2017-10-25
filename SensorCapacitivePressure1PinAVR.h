/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorCapacitivePressure1PinAVR.h

#ifndef _SENSORCAPACITIVEPRESSURE1PINAVR_h
#define _SENSORCAPACITIVEPRESSURE1PINAVR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"

#ifdef __AVR__ // this implementation is AVR specific

// A capacitive pressure sensor made of two plates of conductive foil with an insulating material between them forming a capacitor
// When pressure is applied the capacitance increases
// This implementation measures the time to charge the capacitor to a "HIGH" level, this value is in a linear relationship to the capacitance
// Similar implementations: http://wordpress.codewrite.co.uk/pic/2014/01/25/capacitance-meter-mk-ii/, http://playground.arduino.cc/Code/CapacitiveSensor
// This implementation uses only one pin of the microcontroller. The capacitor bottom plate is conencted to ground
// At all times the capacitor top plate is connected to ground through a 70K to 100K ohm discharge resistor
// When taking a measurement, the capacitor top plate is sensed through a digital pin while at the same time the same pin charges it through a built-in PULLUP resistor (~40K ohm)
class SensorCapacitivePressure1PinAVR : public ISensor
{
public:
	SensorCapacitivePressure1PinAVR(uint8_t pin);
	sensorVal_t getValueInt(); // returns analogRead
	float getValueFloat();
	int getSamplingTime();

private:
	uint8_t mPin;
	// pointers for quick pin/port access
	volatile uint8_t *mPin_port;
	uint8_t mPin_mask;

	const int DISCHARGE_FACTOR = 4; // ratio between discharge time and charge time
	const int AVERAGE_SAMPING_TIME = 50*(DISCHARGE_FACTOR+1); // the time to charge varies depending on how big the capacitor is, value in microseconds
	const int TIMEOUT_ATTEMPTS = 1000; // max number of read attempts to make while waiting for the cap to charge
	const int GAIN = 5; // a gain to make a better use of the expected sensor range
};

#endif // AVR

#endif

