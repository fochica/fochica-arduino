// 
// 
// 

#include "RTCImpl_Sync.h"

RTCImpl_Sync::RTCImpl_Sync()
{
	mPrevMilli = mMilliBase = millis();
	mTimeBase = 0;
}

uint32_t RTCImpl_Sync::getUnixTime()
{
	unsigned long m = millis();

	// detect wraps and rebase
	if (m < mPrevMilli) { // if a wrap happened
		mTimeBase += MILLI_WRAP_FOR_UNIXTIME;
	}
	mPrevMilli = m;

	// apply base
	uint32_t t = mTimeBase;
	if (m >= mMilliBase) // if cur millis is more than base, normal event
		t += ((m - mMilliBase)/1000);
	else // if cur millis is less than base, must have happened due to wrap
		t -= ((mMilliBase - m)/1000);

	return t;
}

void RTCImpl_Sync::setUnixTime(uint32_t t)
{
	mTimeBase = t;
	mPrevMilli = mMilliBase = millis();
}

bool RTCImpl_Sync::isSynced()
{
	return mTimeBase!=0; // synced if there is a value for time base
}
