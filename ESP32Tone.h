// ESP32Tone.h

#ifndef _ESP32TONE_h
#define _ESP32TONE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// ESP32 Arduino implementation currently lacks tone/noTone
// implement using ledc channel 0

#ifdef ESP32 // implementation for ESP32 only
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration=0);
void noTone(uint8_t _pin);
#endif // ESP32

#endif

