// 
// 
// 

#include "SoundManager.h"

void SoundManager::playTone(int tonepin, int freq, int dur, int pause)
{
	tone(tonepin, freq);
	delay(dur);
	noTone(tonepin);
	delay(pause);
}

void SoundManager::setPassiveBuzzer(int pin)
{
	mPassiveBuzzerPin = pin;
	mUsePassiveBuzzer = true;
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
	case BeepType::Error: // generic error, preferably use specific beep
		playTone(mPassiveBuzzerPin, 950, 330, 30);
		playTone(mPassiveBuzzerPin, 1400, 330, 30);
		playTone(mPassiveBuzzerPin, 1800, 330, 30);
		break;
	case BeepType::Start:
		playTone(mPassiveBuzzerPin, 1800, 330, 30);
		break;
	default:
		playTone(mPassiveBuzzerPin, 1800, 500, 30);
		break;
	}
}
