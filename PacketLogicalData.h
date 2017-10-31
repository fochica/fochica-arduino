/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PacketLogicalData.h

#ifndef _PACKETLOGICALDATA_h
#define _PACKETLOGICALDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// could be device capabilities or adapter capabilities
struct Capabilities {
	enum e {
		CanReceivePackets = (1u << 0)
	};
};

struct PacketLogicalData {
	uint32_t deviceUniqueId; // unique device id (not guaranteed to be persistent across boots)
	uint8_t capabilityMask; // Capabilities::e
	uint8_t clientCount; // number of adapters
	uint8_t connectedClientCount; // number of clients connected
	uint8_t seatCount;
	uint8_t sensorCount;
} __attribute__((packed));

#endif

