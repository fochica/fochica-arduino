// IRTC.h

#ifndef _IRTC_h
#define _IRTC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// interface of a real-time clock
// uses unixtime which is defined as the number of seconds that have elapsed since 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970
class IRTC
{
public:
	virtual boolean begin() = 0;
	virtual uint32_t getUnixTime() = 0;
	virtual void setUnixTime(uint32_t t) = 0;
};

#endif

