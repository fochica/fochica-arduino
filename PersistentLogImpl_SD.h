/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PersistentLogImpl_SD.h

#ifndef _PERSISTENTLOGIMPL_SD_h
#define _PERSISTENTLOGIMPL_SD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IPersistentLog.h"

#include <RTClib.h> // for DateTime
#include <SD.h> // for File type, should be the newer SD library from https://github.com/adafruit/SD to suport specifying the SPI pins for soft-SPI
#include "IRTC.h"

class PersistentLogImpl_SD : public IPersistentLog
{
public:
	PersistentLogImpl_SD(IRTC & rtc, int pin, int8_t mosi = -1, int8_t miso = -1, int8_t sck = -1);
	boolean begin();
	Print * open(); // open a stream for writing, caller should close as soon as it is done writing
	void close();
	bool isPresent() { return mPresent; }

private:
	int mPin, mMosi, mMiso, mSck;
	IRTC & mRTC;
	bool mPresent;
	File mFile;
};

#endif

