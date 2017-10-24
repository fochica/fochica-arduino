/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIGVARIATION_h
#define _CONFIGVARIATION_h

// Configuration variations for the Basic build
// https://github.com/fochica/fochica-wiki/wiki/Basic-build
// 1 BLE modules, 1 seat, 2 sensors
class ConfigVariation : public ConfigVariationBase
{
public:
	ConfigVariation();

	virtual void registerSensors(SensorManager & sm);

protected:

private:
	// capacitive touch sensor
	const int CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN = 2; // analog pin#
	const int CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN = 3; // analog pin#

	// chest buckle sensor
	const int REED_SWITCH_SENSOR_PIN = 6;

};

ConfigVariation::ConfigVariation()
{

}

void ConfigVariation::registerSensors(SensorManager & sm)
{
	SensorQtouch * capSense=new SensorQtouch("CapSense", CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN, CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN);
	SensorDigital * digitalReed=new SensorDigital("Reed", REED_SWITCH_SENSOR_PIN, INPUT_PULLUP);

	sm.setSeatCount(1);
	sm.setSensorCount(2);

	capSense->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capSense);
	digitalReed->begin();
	sm.addSensor(0, SensorLocation::Chest, digitalReed);

}

#else
#error Only one ConfigVariation can be included
#endif