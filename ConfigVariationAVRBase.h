/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ConfigVariationAVRBase.h

#ifndef _CONFIGVARIATIONAVRBASE_h
#define _CONFIGVARIATIONAVRBASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ConfigVariationBase.h"
#include "SensorVccAVR.h"
#include "SensorFreeRAMAVR.h"
#include "SensorVoltage.h"

#ifdef __AVR__

// A base abstract class for configuration variations for AVR based devices.
class ConfigVariationAVRBase : public ConfigVariationBase
{
public:
	ConfigVariationAVRBase();

	virtual void registerClientDevices(ClientManager & cm);

	virtual ISensor * getVccSensor() { return new SensorVccAVR("Vcc"); };
	virtual ISensor * getFreeRAMSensor() { return new SensorFreeRAMAVR("SRAM"); };
	virtual ISensor * getBatterySensor() { return new SensorVoltage("Battery", getBatteryVoltageSensorAnalogPin(), getBatteryVoltageSensorResistorToGroundValue(), getBatteryVoltageSensorResistorToVoltageValue()); };

protected:
	// Bluetooth Low Energy (HM-10/CC41 module)
	// Software serial
	const int BLE_RX_PIN = 8;
	const int BLE_TX_PIN = 9;
	const int BLE_STATE_PIN = 7;

	// capacitive pressure sensor
	const int CAPACITIVE_PRESSURE_SENSOR_PIN = A2;

private:
};

#endif // if AVR

#endif
