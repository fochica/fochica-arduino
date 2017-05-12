/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// TechnicalManager.h

#ifndef _TECHNICALMANAGER_h
#define _TECHNICALMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
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
	void setDeviceCurrentSensor(ISensor * sensor) { mDeviceCurrentSensor = sensor; }
	void setFreeRAMSensor(ISensor * sensor) { mFreeRAMSensor = sensor; }

	void work(IClient * client);

private:
	ISensor * mVccSensor;
	ISensor * mCarBatteryVoltageSensor;
	ISensor * mDeviceCurrentSensor;
	ISensor * mFreeRAMSensor;
};

#endif

