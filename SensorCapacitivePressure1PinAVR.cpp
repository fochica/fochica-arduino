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

#include "SensorCapacitivePressure1PinAVR.h"

SensorCapacitivePressure1PinAVR::SensorCapacitivePressure1PinAVR(const char * name, uint8_t pin) : ISensor(name, SensorType::CapacitivePressure)
{
	mPin = pin;
	// optimize access to pin if using AVR platform
	// do this once at setup - https://forum.arduino.cc/index.php?topic=337578.0
	mPin_mask = digitalPinToBitMask(pin);
	mPin_port = portInputRegister(digitalPinToPort(pin));
}

sensorVal_t SensorCapacitivePressure1PinAVR::getValueInt()
{
	int tests, left = TIMEOUT_ATTEMPTS;
	noInterrupts(); // assumes interupts were enabled before
	// Start charging the capacitor with the internal pullup
	pinMode(mPin, INPUT_PULLUP);

	// Charge to a HIGH level, somewhere between 2.6V (practice) and 3V (spec)
	// Best not to use digital/analogRead() here because it's not really quick enough
	// do as little as possible in this loop to get good resolution
	do
	{
		left--;
	} while (((*mPin_port & mPin_mask) == 0) && left>0);

	interrupts();
	pinMode(mPin, INPUT);  //Stop charging
	tests = TIMEOUT_ATTEMPTS - left;

	// Discharge will happen with time
	// Discharge is slower than charge, typically goes through a 100K resistor where charge is a ~40K resistor
	// Charge time is approximately "times" microseconds [us], so use that approximation for discharge delay as well
	delayMicroseconds(tests*DISCHARGE_FACTOR);

	return tests*GAIN;
}

float SensorCapacitivePressure1PinAVR::getValueFloat()
{
	return getValueInt();
}

int SensorCapacitivePressure1PinAVR::getSamplingTime()
{
	return AVERAGE_SAMPING_TIME;
}

#endif