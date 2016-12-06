// RNGUtils.h

#ifndef _RNGUTILS_h
#define _RNGUTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RNGUtils
{
public:
	static unsigned long generateEntropyWithAnalogInputs();
	static void seedWithAnalogInputs();
	static double getGaussian();
	static long getLong(long min, long max);
	static long getLong(long max);
	static long getLong();

private:
	static const int ANALOGS_FOR_SEED = 5;
};

#endif

