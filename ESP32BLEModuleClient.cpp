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

#include "ESP32BLEModuleClient.h"

#ifdef ESP32 // this implementation is ESP32 specific

bool ESP32BLEModuleClient::sendTime(const PacketTime & packet)
{
	return mModule.sendPacket(mId, PacketType::Time, (const byte *)&packet, sizeof(PacketTime));
}

bool ESP32BLEModuleClient::sendTechnicalData(const PacketTechnicalData& packet)
{
	return mModule.sendPacket(mId, PacketType::TechnicalData, (const byte *)&packet, sizeof(PacketTechnicalData));
}

bool ESP32BLEModuleClient::sendSensorData(const PacketSensorData& packet)
{
	return mModule.sendPacket(mId, PacketType::SensorData, (const byte *)&packet, sizeof(PacketSensorData));
}

bool ESP32BLEModuleClient::sendLogicalData(const PacketLogicalData & packet)
{
	return mModule.sendPacket(mId, PacketType::LogicalData, (const byte *)&packet, sizeof(PacketLogicalData));
}

bool ESP32BLEModuleClient::sendCalibrationParams(const PacketCalibrationParams & packet)
{
	return mModule.sendPacket(mId, PacketType::CalibrationParams, (const byte *)&packet, sizeof(PacketCalibrationParams));
}



#endif // ESP32