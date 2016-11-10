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
		ClientConnected,
		ClientDisconnected,
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

	int mPassiveBuzzerPin;
	bool mUsePassiveBuzzer;

	static void playTone(int tonepin, int freq, int dur, int pause);

public:
	static SoundManager& getInstance() {
		static SoundManager singleton;
		return singleton;
	}

	void setPassiveBuzzer(int pin);
	void playBeep(BeepType::e beepType);

};

#endif

