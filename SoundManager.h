/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// SoundManager.h

#ifndef _SOUNDMANAGER_h
#define _SOUNDMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

struct BeepType {
	enum e {
		Start,
		ClientConnected,
		ClientDisconnected,
		SeatCalibrationStart,
		SeatCalibrationEnd,
		SeatCalibrationSuccess,
		SensorStateChange,
		SeatStateChange,
		SeatStateChangeNoClient,
		DischargeProtectionShutdown,
		FallbackReminderAlert,
		Error
	};
};

// This class is a singleton for making sound
// At first support a passive buzzer. In the future add support for other modules.
class SoundManager
{
private:
	// protect constructors to prevent singleton modification
	SoundManager() { mUsePassiveBuzzer = false; }
	SoundManager(const SoundManager &rhs);
	SoundManager & operator=(const SoundManager &rhs);

	uint8_t mPassiveBuzzerPin;
	uint8_t mPassiveBuzzerOffLevel;
	bool mUsePassiveBuzzer;

	static void playTone(int tonepin, int freq, int dur, int pause);

public:
	static SoundManager& getInstance() {
		static SoundManager singleton;
		return singleton;
	}

	void setPassiveBuzzer(uint8_t pin, uint8_t offLevel); // passive buzzer pin and level to set when not making sounds. Depends on the transistor used to drive the buzzer. For NPN (preferred) use LOW and for PNP use HIGH.
	void playBeep(BeepType::e beepType);

};

#endif

