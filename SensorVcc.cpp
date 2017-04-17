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

#include "SensorVcc.h"

SensorVcc::SensorVcc(const char * name) : ISensor(name, SensorType::Vcc)
{
}

// value should be around 5000 (mV)
sensorVal_t SensorVcc::getValueInt()
{
	return getValueFloat() * 1000;
}

// value should be around 5 (V)
float SensorVcc::getValueFloat()
{
	// can't set mux to 14 with analogRead
	// need to set manually
	// Possibly AVR specific
	// cross platform issue
	byte mux = ADMUX; // save current adc mux value
	ADMUX = (DEFAULT << 6) | BANDGAP_REF_PIN; // select 14 in the mux, have to do manually and select AnalogReference=DEFAULT
	delayMicroseconds(SETTLE_DURATION_US);
	bitSet(ADCSRA, ADSC);
	while (bit_is_set(ADCSRA, ADSC));
	word refReading = ADC;
	ADMUX = mux; // restore mux and reference mode (if the mode before was not DEFAULT, we might cause a change in reference mode. does this need a settle time?)

	return (BANDGAP_VOLTAGE * ISensor::MAX_ADC_VALUE) / refReading;
}

int SensorVcc::getSamplingTime()
{
	return SETTLE_DURATION_US+ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
