// PersistentSettings.h

#ifndef _PERSISTENTSETTINGS_h
#define _PERSISTENTSETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
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
		byte seatSensorCalibrationParamsCount;
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
	bool readSeatSensorCalibrationParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, CalibrationParams & cp);
	void writeSeatSensorCalibrationParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, const CalibrationParams & cp);

};

#endif

