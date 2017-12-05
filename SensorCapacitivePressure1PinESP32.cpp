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

#ifdef ESP32 // this implementation is ESP32 specific

#include "DebugStream.h"
#include "SensorCapacitivePressure1PinESP32.h"

SensorCapacitivePressure1PinESP32::SensorCapacitivePressure1PinESP32(uint8_t pin) : ISensor(SensorType::CapacitivePressure)
{
	mPin = pin;
}

sensorVal_t SensorCapacitivePressure1PinESP32::getValueInt()
{
	unsigned int tests, left = TIMEOUT_ATTEMPTS;

	//noInterrupts(); // should we disble interrupts on the ESP32 during this measurement?

	pinMode(mPin, INPUT_PULLUP);
	//uint32_t startCycle = xthal_get_ccount(); // we are measuring cycles here
	// Charge to a HIGH level, somewhere between 1.2V and 1.5V (empirical)
	while (digitalRead(mPin) == LOW && --left>0); // we are counting number of loop iterations here (about 42 cycles per loop)
	//uint32_t endCycle = xthal_get_ccount();
	
	// stop charge, start discharge
	pinMode(mPin, INPUT_PULLDOWN);

	// calculate loops
	tests = TIMEOUT_ATTEMPTS - left; // how many loop iterations

	// discharge duration
	// we charge to about half Vsource, so t(charge)=ln(2)*RC => RC=t(charge)/ln(2)
	// to discharge properly we should discharge for t(dis)=5RC=5*t(charge)/ln(2)=5*loops2us*42/1.44=5*60*loops2us
	delayMicroseconds(clockCyclesToMicroseconds(5*60*tests)); // discharge for same time that was charged, but also leave as pull-down

	// debug
	// note: if this code runs, it will provide additional time for the cap to discharge. something that otherwise does not happen in a real scenario.
	/*
	if (DebugStream) {
		DebugStream->print("SensorCapacitivePressure1PinESP32: ");
		DebugStream->print(endCycle - startCycle);
		DebugStream->print(", ");
		DebugStream->print(tests);
		DebugStream->print(", ");
		DebugStream->println(clockCyclesToMicroseconds(endCycle - startCycle));
	}
	*/

	return tests*GAIN;
}

float SensorCapacitivePressure1PinESP32::getValueFloat()
{
	return getValueInt();
}

int SensorCapacitivePressure1PinESP32::getSamplingTime()
{
	return AVERAGE_SAMPING_TIME;
}

#endif