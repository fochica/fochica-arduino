// TechnicalManager.h

#ifndef _TECHNICALMANAGER_h
#define _TECHNICALMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ISensor.h"
#include "IClient.h"

// Implements collecting and sending of system parameter data, such as electrical parameters, ram, etc
// In the future: Responsible for over dischanrge protection
class TechnicalManager
{
public:
	TechnicalManager();

	void setVccSensor(ISensor * sensor) { mVccSensor = sensor; }
	void setCarBatteryVoltageSensor(ISensor * sensor) { mCarBatteryVoltageSensor = sensor; }
	void setCarBatteryCurrent(ISensor * sensor) { mCarBatteryCurrentSensor = sensor; }
	void setFreeRAMSensor(ISensor * sensor) { mFreeRAMSensor = sensor; }

	void work(IClient * client);

private:
	ISensor * mVccSensor;
	ISensor * mCarBatteryVoltageSensor;
	ISensor * mCarBatteryCurrentSensor;
	ISensor * mFreeRAMSensor;
};

#endif

