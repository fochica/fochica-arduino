/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// DischargeProtectionManager.h

#ifndef _DISCHARGEPROTECTIONMANAGER_h
#define _DISCHARGEPROTECTIONMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DebugStream.h"
#include "PersistentLog.h"
#include "CalibratedSensor.h"
#include "SoundManager.h"

// This class monitors a calibrated sensor of battery voltage and sends a HIGH signal on some been as long as we want to keep using power.
// Once the monitor detects that the voltage is in "low state" it stops emitting the "keep-live" signal causing the discharge protection circuit to stop power to the device.
// If battery voltage sensing circuit is not working, this can cause us to shutdown when there is no need to.
// If the discharge protection circuit is not working, this can cause us to keep running even though we are in what seems to be a "low state".
class DischargeProtectionManager
{
public:
	DischargeProtectionManager(CalibratedSensor & lowChargeSensor, uint8_t keepAlivePin);
	void begin();
	void work();

private:
	CalibratedSensor & mLowChargeSensor;
	uint8_t mKeepAlivePin;
	boolean mShutdownDone;
};

#endif

