/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

// RTC implementation using a DS1307 RTC chip. Uses a battery.
// Can keep time across restarts and when the board is off.
// RTC is synced to time from the clients.
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

