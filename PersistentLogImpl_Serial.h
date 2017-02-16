// PersistentLogImpl_Serial.h

#ifndef _PERSISTENTLOGIMPL_SERIAL_h
#define _PERSISTENTLOGIMPL_SERIAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IPersistentLog.h"

#include "IRTC.h"

// log on top of a serial interface. this implementation expects the caller to initialize the serial interface
class PersistentLogImpl_Serial : public IPersistentLog
{
public:
	PersistentLogImpl_Serial(Stream & stream, IRTC & rtc);
	boolean begin();
	Print * open(); // open a stream for writing, caller should close as soon as it is done writing
	void close();

private:
	IRTC & mRTC;
	Stream & mStream;
	bool mOpen; // for problem detection
};

#endif

