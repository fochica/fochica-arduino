// 
// 
// 

#include "SensorFreeRAM.h"

SensorFreeRAM::SensorFreeRAM(const char * name) : ISensor(name, SensorType::RAM)
{
}

int SensorFreeRAM::getValueInt()
{
	// based on https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
	// Possibly AVR specific
	// cross platform issue
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

float SensorFreeRAM::getValueFloat()
{
	return getValueInt();
}

int SensorFreeRAM::getSamplingTime()
{
	return 0; // no ADC for this sensor
}
