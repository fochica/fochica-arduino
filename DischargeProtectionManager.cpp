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

#include "DischargeProtectionManager.h"

// DPM messages, these are too large for limited flash of smaller AVRs
#ifdef __AVR_ATmega328P__
#define DPM_ALERT F("DPM alert at pin=")
#else
#define DPM_ALERT F("Detecting low charge state and terminating keep-alive signal at pin=")
#endif

DischargeProtectionManager::DischargeProtectionManager(CalibratedSensor & lowChargeSensor, uint8_t keepAlivePin) : mLowChargeSensor(lowChargeSensor)
{
	mKeepAlivePin = keepAlivePin;
	mShutdownDone = false;
}

void DischargeProtectionManager::begin()
{
	// start keep-alive signal so we don't loose power
	pinMode(mKeepAlivePin, OUTPUT);
	digitalWrite(mKeepAlivePin, HIGH);
}

void DischargeProtectionManager::work()
{
	CalibratedSensorState::e state = mLowChargeSensor.getValue();
	if (DebugStream) {
		DebugStream->print(F("DischargeProtectionManager, state="));
		DebugStream->println(state);
	}

	if (state == CalibratedSensorState::A && mShutdownDone==false) { // if low state
		if (DebugStream) {
			DebugStream->print(DPM_ALERT);
			DebugStream->println(mKeepAlivePin);
		}
		if (PersistentLog) {
			Print * f = PersistentLog->open();
			if (f) {
				f->print(DPM_ALERT);
				f->println(mKeepAlivePin);
				PersistentLog->close();
			}
		}
		SoundManager::getInstance().playBeep(BeepType::DischargeProtectionShutdown);
		digitalWrite(mKeepAlivePin, LOW);
		mShutdownDone = true; // prevent these notifications from hapenning if power continues while state is low (meaning we shouldn't shutdown the power supply)
	}
}
