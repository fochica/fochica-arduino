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

#include "SensorQtouch.h"

SensorQtouch::SensorQtouch(const char * name, int analogReadPin, int analogAuxPin) : ISensor(name, SensorType::CapacitiveProximity)
{
	mReadPin = analogReadPin;
	mAuxPin = analogAuxPin;
}

// returns values between 0 and 1024, usually lower than max.
// consider to add GAIN=10 to utilize int range best
sensorVal_t SensorQtouch::getValueInt()
{
	// First measurement  
	// this charges S&H and discharges read cap. then transfer happens to the read cap. new value is measured on S&H. gives adc1 which is subtracted from result!
	int adc1 = makeMeasurement(InToOut);

	// Second measurement - Do everything again with inverted aux / probe pins
	// this discharges S&H and charges the read cap. then transfer happens to the S&H cap. new value is measured on S&H. gives adc2 which is added to the result!
	int adc2 = makeMeasurement(OutToIn);

	return max(adc2 - adc1,0); // return conversion result
}

float SensorQtouch::getValueFloat()
{
	return getValueInt();
}

int SensorQtouch::getSamplingTime()
{
	return 2*(CHARGE_DELAY + TRANSFER_DELAY + ISensor::DEFAULT_ADC_SAMPLING_TIME);
}

// Possibly AVR specific
// cross platform issue
// some implementations online used to read from the aux pin, but my tests have shown this is inferior. Still, keep this mode as an option (altRead)
int SensorQtouch::makeMeasurement(bool dir, bool altRead)
{
	// configure pins as push-pull output
	pinMode(analogInputToDigitalPin(mReadPin), OUTPUT);
	pinMode(analogInputToDigitalPin(mAuxPin), OUTPUT);

	if (dir == InToOut)
	{
		// set aux high to charge the s&h cap and pin low to discharge touch probe cap
		digitalWrite(analogInputToDigitalPin(mReadPin), LOW);
		digitalWrite(analogInputToDigitalPin(mAuxPin), HIGH);
	}
	else // OutToIn
	{
		// set read pin high to charge the touch probe and aux low to discharge s&h cap
		digitalWrite(analogInputToDigitalPin(mReadPin), HIGH);
		digitalWrite(analogInputToDigitalPin(mAuxPin), LOW);
	}

	// charge/discharge s&h cap by connecting it to aux (direction depends on aux pin level/state)
	ADMUX = (ADMUX & ~ADMUX_MASK) | (mAuxPin & ADMUX_MASK); // note, some arduino MCU can have MUX4

	// wait for the charge to transfer
	delayMicroseconds(CHARGE_DELAY); // wait for the touch probe and the s&h cap to be fully charged/discharged 

	// configure pins as input
	// disable the internal pullup to make the ports high Z 
	pinMode(analogInputToDigitalPin(mReadPin), INPUT);
	pinMode(analogInputToDigitalPin(mAuxPin), INPUT);

	// connect touch probe cap to s&h cap to transfer the charge (direction depends on who was charged)
	ADMUX = (ADMUX & ~ADMUX_MASK) | (mReadPin & ADMUX_MASK); // some arduino MCU can have MUX4
	delayMicroseconds(TRANSFER_DELAY); // wait for charge to be transfered  

	// measure
	if (altRead) // "not recommended" alternative mode
		return analogRead(mAuxPin); // why use aux-pin? this methond has a similar span, but about 400units lower... not sure how this works, against documentation.
	return analogRead(mReadPin); // preferred
}
