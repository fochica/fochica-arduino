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

#include "ConfigVariationBase.h"
#include "SoftwareSerial.h"
#include "GenericBLEModuleClient.h"

ConfigVariationBase::ConfigVariationBase()
{
}

void ConfigVariationBase::registerClientDevices(ClientManager & cm)
{
	// communication devices
#ifdef HAVE_HWSERIAL1
	GenericBLEModuleClient * ble=new GenericBLEModuleClient(Serial1, BLE_STATE_PIN);
#else
	SoftwareSerial * bleSerial=new SoftwareSerial(BLE_RX_PIN, BLE_TX_PIN);
	GenericBLEModuleClient * ble=new GenericBLEModuleClient(*bleSerial, BLE_STATE_PIN);
#endif

	cm.setDeviceCount(1);
	ble->begin();
	cm.addDevice(ble);
	// this call is only done at init, the module is never deallocated
}
