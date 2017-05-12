/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// RTCImpl_Sync.h

#ifndef _RTCIMPL_SYNC_h
#define _RTCIMPL_SYNC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
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

