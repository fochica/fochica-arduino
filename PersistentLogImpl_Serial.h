/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PersistentLogImpl_Serial.h

#ifndef _PERSISTENTLOGIMPL_SERIAL_h
#define _PERSISTENTLOGIMPL_SERIAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IPersistentLog.h"

#include "IRTC.h"

// log on top of a serial interface. this implementation expects the caller to initialize the serial interface
class PersistentLogImpl_Serial : public IPersistentLog
{
public:
	PersistentLogImpl_Serial(Stream & stream, IRTC & rtc);
	boolean begin();
	Print * open(); // open a stream for writing, caller should close as soon as it is done writing
	void close();

private:
	IRTC & mRTC;
	Stream & mStream;
	bool mOpen; // for problem detection
};

#endif

