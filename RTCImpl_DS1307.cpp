// 
// 
// 

#include "RTCImpl_DS1307.h"

RTCImpl_DS1307::RTCImpl_DS1307()
{
	mInitialized == false;
}

boolean RTCImpl_DS1307::begin()
{
	mInitialized = true;
	return mRTC.begin();
}

uint32_t RTCImpl_DS1307::getUnixTime()
{
	if(mInitialized)
		return mRTC.now().unixtime();
	return 0;
}

void RTCImpl_DS1307::setUnixTime(uint32_t t)
{
	if(mInitialized)
		mRTC.adjust(t);
}
