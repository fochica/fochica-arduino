/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
