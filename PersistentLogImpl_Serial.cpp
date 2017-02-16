// 
// 
// 

#include "PersistentLogImpl_Serial.h"
#include "DebugStream.h"

PersistentLogImpl_Serial::PersistentLogImpl_Serial(Stream & stream, IRTC & rtc) : mStream(stream), mRTC(rtc)
{
	mOpen = false;
}

boolean PersistentLogImpl_Serial::begin()
{
	// do nothing, expct the caller to initialize the serial
	return true;
}

Print * PersistentLogImpl_Serial::open()
{
	// check if already open
	if (mOpen) {
		if (DebugStream)
			DebugStream->println(F("Openning the serial persistent log while it is already open. Warning."));
	}

	DateTime dt(mRTC.getUnixTime());
	printDate(mStream, dt);
	mStream.print(F(", "));
	mOpen = true;

	return &mStream;
}

void PersistentLogImpl_Serial::close()
{
	// do nothing, serial log is kept open all the time
	mOpen = false;
}
