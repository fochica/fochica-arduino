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

#include "TechnicalManager.h"

TechnicalManager::TechnicalManager()
{
	mVccSensor = mDeviceCurrentSensor = mCarBatteryVoltageSensor = mFreeRAMSensor = NULL;
}

void TechnicalManager::work(IClient * client)
{
	// send technical packet
	PacketTechnicalData packet;
	packet.vcc = mVccSensor ? mVccSensor->getValueInt() : 0;
	packet.deviceCurrent = mDeviceCurrentSensor ? mDeviceCurrentSensor->getValueInt() : 0;
	packet.carBatteryVoltage = mCarBatteryVoltageSensor ? mCarBatteryVoltageSensor->getValueInt() : 0;
	packet.freeRAM = mFreeRAMSensor ? mFreeRAMSensor->getValueInt() : 0;
	client->sendTechnicalData(packet);
}
