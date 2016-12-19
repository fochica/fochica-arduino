// IPersistentLog.h

#ifndef _IPERSISTENTLOG_h
#define _IPERSISTENTLOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <RTClib.h> // for DateTime

// interface of a persistent log, such an SD card
class IPersistentLog
{
public:
	virtual boolean begin() = 0;
	virtual Print & open() = 0;
	virtual void close(Print &) = 0;

protected:
	void printDate(Print & out, const DateTime & d);
};

#endif

