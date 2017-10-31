/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ConfigVariationAVRBase.h

#ifndef _CONFIGVARIATIONESP32BASE_h
#define _CONFIGVARIATIONESP32BASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ConfigVariationBase.h"
#include "SensorVoltage.h"
#include "SensorFreeRAMESP32.h"


#ifdef ESP32

// A base abstract class for configuration variations for ESP32 based devices.
class ConfigVariationESP32Base : public ConfigVariationBase
{
public:
	ConfigVariationESP32Base();

	virtual ISensor * getVccSensor() { return NULL; };
	virtual ISensor * getFreeRAMSensor() { return new SensorFreeRAMESP32(); };
	virtual ISensor * getBatterySensor() { return NULL; };

	virtual void registerClientDevices(ClientManager & cm);

protected:

private:
};

#endif /// if ESP32

#endif
