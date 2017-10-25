/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorQtouch.h

#ifndef _SENSORQTOUCH_h
#define _SENSORQTOUCH_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ISensor.h"

#ifdef __AVR__ // this implementation is AVR specific

// Sensor that uses capacitance to sense what is near the sensor
// Principle of operation is as in Atmel's QTouchADC technology, but not an official implementation
// http://www.atmel.com/images/doc8497.pdf - Atmel AVR3001: QTouchADC Measurement and Tuning
// Usage example in http://www.electrobob.com/sit-up/ project
// Requires two pins. The secondary pin (aux) can be a main pin of another capacitive sensor instance.
// The secondary aux pin is special for this sensor logic and is not to be confused with MCU ADC reference
class SensorQtouch : public ISensor
{
public:
	SensorQtouch(uint8_t analogReadPin, uint8_t analogAuxPin);
	sensorVal_t getValueInt(); // returns V*[internal coeff], higher when capacitance is higher
	float getValueFloat();
	int getSamplingTime();

private:
	uint8_t mReadPin;
	uint8_t mAuxPin;

	enum TransferDirection {
		InToOut, // charge internal cap, then share charge with external cap
		OutToIn // charge external cap, then share charge with internal cap
	};

	const int CHARGE_DELAY = 10; // time it takes for the capacitor to get charged/discharged in microseconds
	const int TRANSFER_DELAY = 5; // time it takes for the capacitors to exchange charge
	const byte ADMUX_MASK = 0b00011111; // mask the mux bits in the ADMUX register - including for MCU with MUX4

	int makeMeasurement(bool dir, bool altRead = false);
};

#endif // AVR

#endif

