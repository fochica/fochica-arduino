// RTCImpl_Sync.h

#ifndef _RTCIMPL_SYNC_h
#define _RTCIMPL_SYNC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IRTC.h"

// RTC implementation that doesn't use a physical RTC module.
// Can't keep right time across reboots but once synced with client it will keep proper time.
// As simple measure is introduced to cope with millis overflow (happens every 50 days or so).
class RTCImpl_Sync : public IRTC
{
public:
	RTCImpl_Sync();
	boolean begin() {};
	uint32_t getUnixTime();
	void setUnixTime(uint32_t t);
	bool isSynced();

private:
	unsigned long mMilliBase; // ms at the baseline
	uint32_t mTimeBase; // unix time at the baseline
	unsigned long mPrevMilli; // prev milli values, helps catch wraps. will be reliable only if we perform getUnixTime at least once per wrap
	const uint32_t MILLI_WRAP_FOR_UNIXTIME = 4294967; // 2^32 / 1000 .  the value in seconds that we need to add to time base if millis wrap
};


#endif

