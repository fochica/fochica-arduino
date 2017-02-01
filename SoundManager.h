// SoundManager.h

#ifndef _SOUNDMANAGER_h
#define _SOUNDMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
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
		SensorStateChange,
		SeatStateChange,
		SeatStateChangeNoClient,
		DischargeProtectionShutdown,
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

