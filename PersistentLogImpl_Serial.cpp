// 
// 
// 

#include "PersistentLogImpl_Serial.h"

PersistentLogImpl_Serial::PersistentLogImpl_Serial(Stream & stream, IRTC & rtc) : mStream(stream), mRTC(rtc)
{
}

boolean PersistentLogImpl_Serial::begin()
{
	// do nothing, expct the caller to initialize the serial
	return true;
}

Print & PersistentLogImpl_Serial::open()
{
	DateTime dt(mRTC.getUnixTime());
	printDate(mStream, dt);
	mStream.print(F(", "));

	return mStream;
}

void PersistentLogImpl_Serial::close(Print & print)
{
	// do nothing, serial log is kept open all the time
}
