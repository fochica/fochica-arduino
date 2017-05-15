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
