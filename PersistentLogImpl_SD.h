// PersistentLogImpl_SD.h

#ifndef _PERSISTENTLOGIMPL_SD_h
#define _PERSISTENTLOGIMPL_SD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IPersistentLog.h"

#include <RTClib.h> // for DateTime
#include "IRTC.h"

class PersistentLogImpl_SD : public IPersistentLog
{
public:
	PersistentLogImpl_SD(int pin, IRTC & rtc);
	boolean begin();
	Print & open(); // open a stream for writing, caller should close as soon as it is done writing
	void close(Print & print);
	bool isPresent() { return mPresent; }

private:
	int mPin;
	IRTC & mRTC;
	bool mPresent;

};

#endif

