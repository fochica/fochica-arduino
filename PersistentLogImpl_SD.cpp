// 
// 
// 

#include "PersistentLogImpl_SD.h"

#include <SD.h> // for file
#include "DebugStream.h"


PersistentLogImpl_SD::PersistentLogImpl_SD(int pin, IRTC & rtc) : mRTC(rtc)
{
	mPin = pin;
	mPresent = false;
}

boolean PersistentLogImpl_SD::begin()
{
	mPresent=SD.begin(mPin);
	if (DebugStream) {
		if(mPresent) {
			DebugStream->println(F("SD card failed, or not present"));
		} else {
			DebugStream->println(F("SD card initialized."));
		}
	}
	return mPresent;
}

Print & PersistentLogImpl_SD::open()
{
	// make file name
	DateTime dt(mRTC.getUnixTime());
	char fileName[13]; // 8.3 and null
	snprintf(fileName, sizeof(fileName), "%04d%02d%02d.txt", dt.year(), dt.month(), dt.day());

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	File dataFile = SD.open(fileName, FILE_WRITE);

	// if the file is available, just return it
	if (!dataFile && DebugStream) { // if the file isn't open, error
		DebugStream->print(F("Error opening SD file for writing: "));
		DebugStream->println(fileName);
	}
	else {
		printDate(dataFile, dt);
		dataFile.print(F(", "));
	}

	return dataFile;
}

void PersistentLogImpl_SD::close(Print & print)
{
	((File &)print).close();
}
