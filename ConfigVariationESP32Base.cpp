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

#ifdef ESP32

#include "ConfigVariationESP32Base.h"
#include "ESP32BLEModule.h"

//#define USE_GENERIC_BLE
#ifdef USE_GENERIC_BLE
#include "GenericBLEModuleClient.h"
HardwareSerial Serial2(2); // Tx2 17, Rx2 16
#endif

ConfigVariationESP32Base::ConfigVariationESP32Base()
{
}

void ConfigVariationESP32Base::registerClientDevices(ClientManager & cm)
{
#ifdef USE_GENERIC_BLE
	GenericBLEModuleClient * ble = new GenericBLEModuleClient(Serial2, 5);
	cm.setDeviceCount(1);
	ble->begin();
	cm.addDevice(ble);
#else
	ESP32BLEModule & ble = ESP32BLEModule::getInstance();
	cm.setDeviceCount(ESP32BLEModule::MAX_CLIENTS);
	for (int i = 0; i < ESP32BLEModule::MAX_CLIENTS; i++) {
		cm.addDevice(ble.getClient(i)); // no need to call begin() for these ESP32 clients as they are "virtual", but might be needed to be added in the future
	}
#endif
	// this call is only done at init, the module is never deallocated
}

#endif