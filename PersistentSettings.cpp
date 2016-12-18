// 
// 
// 

#include "PersistentSettings.h"

#include <EEPROM.h>
#include "DebugStream.h"

void PersistentSettings::init()
{
	// read header
	EEPROM.get(EEPROM_START, mHeader);
	if (mHeader.token != TOKEN || mHeader.version != SCHEMA_VERSION) { // if bad or unknown data in EEPROM
		mHeader.token = TOKEN;
		mHeader.version = SCHEMA_VERSION;
		mHeader.seatSensorCalibrationParamsCount = 0;
		EEPROM.put(EEPROM_START, mHeader);
	}
	if (DebugStream) {
		DebugStream->print(F("Loaded header of EEPROM with entries: "));
		DebugStream->println(mHeader.seatSensorCalibrationParamsCount);
	}
}

int PersistentSettings::getAddressOfSSCP(int id)
{
	int address = EEPROM_START + sizeof(PersistentSettingsHeader);
	address += (id*(sizeof(PersistentSettingsSSCPHeader) + sizeof(CalibrationParams)));
	return address;
}

bool PersistentSettings::readSeatSensorCalibrationParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, CalibrationParams & cp)
{
	if (sensorId >= mHeader.seatSensorCalibrationParamsCount) // if this sensorId is not stored
		return false;

	// read SSCP header
	int address = getAddressOfSSCP(sensorId);
	PersistentSettingsSSCPHeader cpHeader;
	EEPROM.get(address, cpHeader);
	if (cpHeader.seatId != seatId || cpHeader.sensorType != sensorType || cpHeader.sensorLocation != sensorLocation)
		return false;

	// read CP
	EEPROM.get(address + sizeof(PersistentSettingsSSCPHeader), cp);
	return true;
}

void PersistentSettings::writeSeatSensorCalibrationParams(sensorCount_t sensorId, seatCount_t seatId, SensorType::e sensorType, SensorLocation::e sensorLocation, const CalibrationParams & cp)
{
	if (sensorId > mHeader.seatSensorCalibrationParamsCount) { // if need to pad entries
		int itemsToPad = sensorId - mHeader.seatSensorCalibrationParamsCount;
		int address= getAddressOfSSCP(mHeader.seatSensorCalibrationParamsCount); // start of items to pad
		
		PersistentSettingsSSCPHeader pad; // define an entry that is not valid, should not match any valid read attempt
		pad.seatId = -1;
		pad.sensorLocation = SensorLocation::Virtual; 
		pad.sensorType = SensorType::Aggregative;
		
		for (int i = 0; i < itemsToPad; i++) {
			EEPROM.put(address, pad);
		}
	}

	// write current entry
	int address = getAddressOfSSCP(sensorId);

	PersistentSettingsSSCPHeader cpHeader;
	cpHeader.seatId = seatId;
	cpHeader.sensorLocation = sensorLocation;
	cpHeader.sensorType = sensorType;
	
	EEPROM.put(address, cpHeader);
	EEPROM.put(address + sizeof(PersistentSettingsSSCPHeader), cp);

	// update header if needed (when we added entries)
	if (sensorId >= mHeader.seatSensorCalibrationParamsCount) {
		mHeader.seatSensorCalibrationParamsCount = sensorId + 1;
		EEPROM.put(EEPROM_START, mHeader);
	}
}
