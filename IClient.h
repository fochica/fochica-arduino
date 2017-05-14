/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// IClient.h

#ifndef _ICLIENT_h
#define _ICLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketTime.h"
#include "PacketTechnicalData.h"
#include "PacketSensorData.h"
#include "PacketLogicalData.h"
#include "PacketCalibrationParams.h"

// Defines an interface to a client, which is typically a smartphone connected over BLE.
class IClient
{
public:
	virtual bool sendTime(const PacketTime& packet) = 0;
	virtual bool sendTechnicalData(const PacketTechnicalData& packet) = 0;
	virtual bool sendSensorData(const PacketSensorData& packet) = 0;
	virtual bool sendLogicalData(const PacketLogicalData& packet) = 0;
	virtual bool sendCalibrationParams(const PacketCalibrationParams& packet) = 0;
};

#endif

