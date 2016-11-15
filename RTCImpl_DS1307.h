// RTCImpl_DS1307.h

#ifndef _RTCIMPL_DS1307_h
#define _RTCIMPL_DS1307_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <RTClib.h>
#include "IRTC.h"

class RTCImpl_DS1307 : public IRTC
{
public:
	RTCImpl_DS1307();
	boolean begin();
	uint32_t getUnixTime();
	void setUnixTime(uint32_t t);

private:
	RTC_DS1307 mRTC;
	bool mInitialized;
};

#endif

