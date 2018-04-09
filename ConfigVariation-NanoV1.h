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

#ifndef ARDUINO_AVR_NANO
#error This configuration is for Nano build settings
#endif

#include "ConfigVariationAVRBase.h"
#include "SensorCapacitivePressure1PinAVR.h"
#include "GenericBLEModuleClient.h"

// Configuration variations for the Nano V1 build
// https://hackaday.io/project/20902/log/68057-finals-submission-and-the-nano-build
// 2 BLE modules, 2 seats, 2 sensors
class ConfigVariation : public ConfigVariationAVRBase
{
public:
	ConfigVariation();

	virtual Nullable<uint8_t> getAliveLedPin() { return Nullable<uint8_t>(LED_BUILTIN); } // "alive" led, flashes during operation. use built-in LED typically on pin 13.

	virtual void registerClientDevices(ClientManager & cm);
	virtual void registerSensors(SensorManager & sm);

	virtual StartModeDetector * getStartModeDetector() {
		return new StartModeDetector(START_MODE_CHARGE_PIN, START_MODE_MEASURE_PIN, START_MODE_RESTART_TH, START_MODE_POWER_CYCLE_TH);
	};

protected:

private:
	// module #2
	const int BLE2_RX_PIN = 2;
	const int BLE2_TX_PIN = 3;
	const int BLE2_STATE_PIN = 5;

	// pressure sensors
	const int CAPACITIVE_PRESSURE_SENSOR_PIN = A2;
	const int CAPACITIVE_PRESSURE_SENSOR2_PIN = A3;

	// start mode
	const int START_MODE_CHARGE_PIN = A0;
	const int START_MODE_MEASURE_PIN = A0;
	const int START_MODE_RESTART_TH = 500;
	const int START_MODE_POWER_CYCLE_TH = 40;

};

ConfigVariation::ConfigVariation()
{

}

void ConfigVariation::registerClientDevices(ClientManager & cm)
{
	// communication devices
#ifdef HAVE_HWSERIAL1
	GenericBLEModuleClient * ble = new GenericBLEModuleClient(Serial1, BLE_STATE_PIN);
#else
	SoftwareSerial * bleSerial = new SoftwareSerial(BLE_RX_PIN, BLE_TX_PIN);
	GenericBLEModuleClient * ble = new GenericBLEModuleClient(*bleSerial, BLE_STATE_PIN);
#endif

#ifdef HAVE_HWSERIAL2
	GenericBLEModuleClient * ble2=new GenericBLEModuleClient(Serial2, BLE2_STATE_PIN);
#else
	SoftwareSerial * bleSerial2=new SoftwareSerial(BLE2_RX_PIN, BLE2_TX_PIN);
	GenericBLEModuleClient * ble2=new GenericBLEModuleClient(*bleSerial2, BLE2_STATE_PIN);
#endif

	cm.setDeviceCount(2);
	ble->begin();
	cm.addDevice(ble);
	ble2->begin();
	cm.addDevice(ble2);
	// this call is only done at init, the modules are never deallocated
}

void ConfigVariation::registerSensors(SensorManager & sm)
{
	SensorCapacitivePressure1PinAVR * capPressure = new SensorCapacitivePressure1PinAVR(CAPACITIVE_PRESSURE_SENSOR_PIN);
	SensorCapacitivePressure1PinAVR * capPressure2 = new SensorCapacitivePressure1PinAVR(CAPACITIVE_PRESSURE_SENSOR2_PIN);

	sm.setSeatCount(2);
	sm.setSensorCount(2);

	capPressure->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capPressure);
	capPressure->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, capPressure2);
}

#else
#error Only one ConfigVariation can be included
#endif