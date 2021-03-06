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

#ifdef  ARDUINO_AVR_MEGA2560
#define SUPPORT_SD_MODULE // The SD library uses over 0.5KB of RAM and lots of Flash memory. The SD module is an SPI devices and takes 4 pins. Practical use is only possible of larger bords, such as the Arduino Mega, not Arduino Uno.
#endif

#ifdef SUPPORT_SD_MODULE // include the code only if support should be included

#include "PersistentLogImpl_SD.h"

#include "DebugStream.h"


PersistentLogImpl_SD::PersistentLogImpl_SD(IRTC & rtc, int pin, int8_t mosi = -1, int8_t miso = -1, int8_t sck = -1) : mRTC(rtc)
{
	mPin = pin;
	mMiso = miso;
	mMosi = mosi;
	mSck = sck;
	mPresent = false;
}

boolean PersistentLogImpl_SD::begin()
{
	mPresent=SD.begin(mPin, mMosi, mMiso, mSck); // the additional parameters are supported in the newer SD library from https://github.com/adafruit/SD
	if (DebugStream) {
		if(mPresent==false) {
			DebugStream->println(F("SD card failed, or not present"));
		} else {
			DebugStream->println(F("SD card initialized."));
		}
	}
	return mPresent;
}

Print * PersistentLogImpl_SD::open()
{
	// check if can be used
	if (!isPresent())
		return NULL;

	// check if already open
	if (mFile) {
		if(DebugStream)
			DebugStream->println(F("Error opening SD file, another file already open."));
		return NULL;
	}

	// make file name
	DateTime dt(mRTC.getUnixTime());
	char fileName[13]; // 8.3 and null
	snprintf(fileName, sizeof(fileName), "%04d%02d%02d.txt", dt.year(), dt.month(), dt.day());
	//if (DebugStream)
	//	DebugStream->println(fileName);

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another or re-openning this one.
	mFile = SD.open(fileName, FILE_WRITE);

	// if the file was not opened correctly, print an error
	if (!mFile) { // if the file isn't open, error
		if (DebugStream) {
			DebugStream->print(F("Error opening SD file for writing: "));
			DebugStream->println(fileName);
		}
		return NULL;
	}
	else {
		size_t s=printDate(mFile, dt); // note, this will perform several "print" operations. each operation takes a few ms for SD file object. consider to write to a memory output device and then flush to file in ope operation.
		s+=mFile.print(F(", "));
		if (s == 0) { // we couldn't write anything, assume persistent error or no free space
			mFile.close();
			mPresent = false; // disable further use to prevent system slowdown, can later implement another behaviour such as rotating log files
			if (DebugStream) {
				DebugStream->println(F("Can't write to SD file, assuming card full and shutting down logging"));
			}
			return NULL;
		}
	}

	return &mFile;
}

void PersistentLogImpl_SD::close()
{
	mFile.close();
}

#endif // SUPPORT_SD_MODULE