// 
// 
// 

#include "SoundManager.h"
#include "DebugStream.h"

void SoundManager::playTone(int tonepin, int freq, int dur, int pause)
{
	tone(tonepin, freq);
	delay(dur);
	noTone(tonepin);
	delay(pause);
}

void SoundManager::setPassiveBuzzer(uint8_t pin, uint8_t offLevel)
{
	mPassiveBuzzerPin = pin;
	mUsePassiveBuzzer = true;
	mPassiveBuzzerOffLevel = offLevel;

	pinMode(pin, OUTPUT);
	digitalWrite(pin, offLevel);
}

void SoundManager::playBeep(BeepType::e beepType)
{
	if (mUsePassiveBuzzer == false)
		return;

	switch (beepType)
	{
	case BeepType::ClientConnected:
		playTone(mPassiveBuzzerPin, 950, 330, 30);
		break;
	case BeepType::ClientDisconnected:
		playTone(mPassiveBuzzerPin, 1400, 330, 30);
		break;
	case BeepType::SeatStateChangeNoClient:
	case BeepType::Error: // generic error, preferably use specific beep
		playTone(mPassiveBuzzerPin, 950, 330, 30);
		playTone(mPassiveBuzzerPin, 1400, 330, 30);
		playTone(mPassiveBuzzerPin, 1800, 330, 30);
		break;
	case BeepType::Start:
		playTone(mPassiveBuzzerPin, 1800, 200, 10);
		playTone(mPassiveBuzzerPin, 1400, 200, 10);
		playTone(mPassiveBuzzerPin, 950, 200, 400);
		break;
	case BeepType::SeatCalibrationStart:
		playTone(mPassiveBuzzerPin, 2000, 330, 30);
		break;
	case BeepType::SeatCalibrationEnd:
		playTone(mPassiveBuzzerPin, 3000, 330, 30);
		break;
	case BeepType::SensorStateChange:
		playTone(mPassiveBuzzerPin, 2000, 50, 0);
		playTone(mPassiveBuzzerPin, 3000, 50, 0);
		break;
	case BeepType::SeatStateChange:
		playTone(mPassiveBuzzerPin, 3000, 50, 0);
		playTone(mPassiveBuzzerPin, 4000, 50, 0);
		break;
	case BeepType::DischargeProtectionShutdown:
		playTone(mPassiveBuzzerPin, 950, 400, 50);
		playTone(mPassiveBuzzerPin, 950, 400, 50);
		playTone(mPassiveBuzzerPin, 950, 400, 50);
		break;
	case BeepType::FallbackReminderAlert:
		playTone(mPassiveBuzzerPin, 400, 100, 30);
		break;
	default:
		playTone(mPassiveBuzzerPin, 1800, 500, 30);
		break;
	}

	// put pin back to default mode
	digitalWrite(mPassiveBuzzerPin, mPassiveBuzzerOffLevel);

	// log
	if (DebugStream) {
		DebugStream->print(F("Playing Beep #"));
		DebugStream->println(beepType);
	}
}
