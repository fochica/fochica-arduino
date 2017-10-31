// 
// 
// 

#include "ESP32Tone.h"

#ifdef ESP32 // implementation for ESP32 only

#define CHANNEL 0

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
	ledcWriteTone(CHANNEL, frequency);
	ledcAttachPin(_pin, CHANNEL);
	delay(duration);
}

void noTone(uint8_t _pin)
{
	ledcWriteTone(CHANNEL, 0);
}

#endif