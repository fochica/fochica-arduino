// 
// 
// 

#include "SensorFreeRAM.h"

SensorFreeRAM::SensorFreeRAM(const char * name) : ISensor(name, SensorType::RAM)
{
}

// should be around 1K-2K for Uno
sensorVal_t SensorFreeRAM::getValueInt()
{
	// based on https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
	// Possibly AVR specific
	// cross platform issue
	extern int __heap_start, *__brkval; // start and end of the heap area
	int v; // variable on the stack, tells us stack boundary
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

void SensorFreeRAM::dumpSRAMBounds(Stream & s)
{
	extern int __heap_start, *__brkval; // start and end of the heap area
	int v; // variable on the stack, tells us stack boundary
	s.print(F("&__heap_start: "));
	s.println((int)&__heap_start);
	s.print(F("__brkval: "));
	s.println((int)__brkval);
	s.print(F("stack: "));
	s.println((int)&v);

}

void SensorFreeRAM::dumpSRAMContent(Stream &s)
{
	// dump all addresses
	for (byte * p = (byte *)RAMSTART; p < (byte *)RAMEND; p++) {
		//s.write(*p);
		s.print((int)p);
		s.print(F(", "));
		if(*p>' ')
			s.write(*p);
		s.print(F(", "));
		s.println((int)*p);
	}
}
