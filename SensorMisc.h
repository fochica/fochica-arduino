/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SensorMisc.h

#ifndef _SENSORMISC_h
#define _SENSORMISC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef uint8_t seatCount_t;
typedef uint8_t sensorCount_t;

struct SensorLocation {
	enum e { Virtual, UnderSeat, Chest, Above };
};

struct SensorState { // relevant for seat sensors
	enum e {
		None, // no sensors to form an opinion for aggregated sensor or initial state for a single sensor
		Occupied,
		Empty,
		NotCalibrated,
		Disabled,
		// aggregated states
		Stabilizing, // disagreement but still in time window to be resolved.
		SensorConflict, // disagreement past time window
		Count
	};
};

struct SensorActivityMode {
	enum e { Active, Deactivated, Disabled }; // Deactivated: don't take part in aggregated state, Disabled: don't even take readings
};

#endif

