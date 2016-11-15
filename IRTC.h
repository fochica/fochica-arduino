// IRTC.h

#ifndef _IRTC_h
#define _IRTC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// interface of a real-time clock
class IRTC
{
public:
	virtual boolean begin() = 0;
	virtual uint32_t getUnixTime() = 0;
	virtual void setUnixTime(uint32_t t) = 0;
};

#endif

