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

#include "ConfigVariationAVRBase.h"
#include "SensorQtouch.h"
#include "SensorDigital.h"
#include "GenericBLEModuleClient.h"

// Configuration variations for the Uno Shield V1 build
// https://hackaday.io/project/20902/log/60964-goodbye-breadboard
// 2 BLE modules, 2 seats, 4 sensors, discharge protection and "alive" LED
class ConfigVariation : public ConfigVariationAVRBase
{
public:
	ConfigVariation();

	virtual Nullable<uint8_t> getAliveLedPin() { return Nullable<uint8_t>(LED_BUILTIN); } // "alive" led, flashes during operation. use built-in LED typically on pin 13.
	virtual DischargeProtectionManager * getDischargeProtectionManager(ISensor * batterySensor);

	virtual void registerClientDevices(ClientManager & cm);
	virtual void registerSensors(SensorManager & sm);

protected:

private:
	// module #2
	const int BLE2_RX_PIN = 2;
	const int BLE2_TX_PIN = 3;
	const int BLE2_STATE_PIN = 5;

	// capacitive touch sensor
	const int CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN = 2; // analog pin#
	const int CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN = 3; // analog pin#

	// chest buckle sensor
	const int REED_SWITCH_SENSOR_PIN = 6;

	// capacitive touch sensor
	const int CAPACITIVE_TOUCH_SENSOR2_READ_ANALOG_PIN = 3; // analog pin#
	const int CAPACITIVE_TOUCH_SENSOR2_AUX_ANALOG_PIN = 2; // analog pin#

	// chest buckle sensor
	const int REED_SWITCH_SENSOR2_PIN = A0;

	// Discharge protection parameters
	const int DISCHARGE_PROTECTION_KEEPALIVE_PIN = 10;
};

ConfigVariation::ConfigVariation()
{

}

DischargeProtectionManager * ConfigVariation::getDischargeProtectionManager(ISensor * batterySensor)
{
	if (batterySensor == NULL) // must have data about battery charge to perform protection
		return NULL;
	CalibratedSensor * lowBatCharge=new CalibratedSensor(batterySensor, DISCHARGE_PROTECTION_ALPHA, DISCHARGE_PROTECTION_LOW_CHARGE_TH, DISCHARGE_PROTECTION_HIGH_CHARGE_TH, CalibratedSensorState::B, DISCHARGE_PROTECTION_HIGH_CHARGE_TH); // set to start in state=B="high charge"
	return new DischargeProtectionManager(*lowBatCharge, DISCHARGE_PROTECTION_KEEPALIVE_PIN);
}

void ConfigVariation::registerClientDevices(ClientManager & cm)
{
	// communication devices (also supports mega with additional hardware serials)
#ifdef HAVE_HWSERIAL1
	GenericBLEModuleClient * ble = new GenericBLEModuleClient(Serial1, BLE_STATE_PIN);
#else
	SoftwareSerial * bleSerial = new SoftwareSerial(BLE_RX_PIN, BLE_TX_PIN);
	GenericBLEModuleClient * ble = new GenericBLEModuleClient(*bleSerial, BLE_STATE_PIN);
#endif

#ifdef HAVE_HWSERIAL2
	GenericBLEModuleClient * ble2 = new GenericBLEModuleClient(Serial2, BLE2_STATE_PIN);
#else
	SoftwareSerial * bleSerial2 = new SoftwareSerial(BLE2_RX_PIN, BLE2_TX_PIN);
	GenericBLEModuleClient * ble2 = new GenericBLEModuleClient(*bleSerial2, BLE2_STATE_PIN);
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
	SensorQtouch * capSense = new SensorQtouch("CapSense", CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN, CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN);
	SensorDigital * digitalReed = new SensorDigital("Reed", REED_SWITCH_SENSOR_PIN, INPUT_PULLUP);
	SensorQtouch * capSense2 = new SensorQtouch("CapSense2", CAPACITIVE_TOUCH_SENSOR2_READ_ANALOG_PIN, CAPACITIVE_TOUCH_SENSOR2_AUX_ANALOG_PIN);
	SensorDigital * digitalReed2 = new SensorDigital("Reed2", REED_SWITCH_SENSOR2_PIN, INPUT_PULLUP);

	sm.setSeatCount(2);
	sm.setSensorCount(4);

	capSense->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capSense);
	digitalReed->begin();
	sm.addSensor(0, SensorLocation::Chest, digitalReed);

	capSense2->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, capSense2);
	digitalReed2->begin();
	sm.addSensor(1, SensorLocation::Chest, digitalReed2);
}

#else
#error Only one ConfigVariation can be included
#endif