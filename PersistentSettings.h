/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PersistentSettings.h

#ifndef _PERSISTENTSETTINGS_h
#define _PERSISTENTSETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SensorManager.h"

// This class is a singleton for saving config to persistent EEPROM memory
class PersistentSettings
{
private:
	// protect constructors to prevent singleton modification
	PersistentSettings() { init(); }
	PersistentSettings(const PersistentSettings &rhs);
	PersistentSettings & operator=(const PersistentSettings &rhs);

	const int SCHEMA_VERSION = 1;
	const int EEPROM_START = 0;
	const long TOKEN = 1594354L; // some random token to identify that we use this EEPROM

	struct PersistentSettingsHeader {
		long token;
		int version;
		byte seatSensorPersistentParamsCount;
	};

	struct PersistentSettingsSSCPHeader {
		seatCount_t seatId;
		SensorType::e sensorType; 
		SensorLocation::e sensorLocation;
	};

	// pivate fields
	PersistentSettingsHeader mHeader;

	// private methods
	void init();
	int getAddressOfSSCP(int id);

public:
	static PersistentSettings& getInstance() {
		static PersistentSettings singleton;
		return singleton;
	}

	// public methods
	bool readSeatSensorPersistentParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, SensorPersistentParams & pp);
	void writeSeatSensorPersistentParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, const SensorPersistentParams & pp);

};

#endif

