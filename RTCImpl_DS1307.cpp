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
