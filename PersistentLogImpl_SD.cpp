// 
// 
// 

#include "PersistentLogImpl_SD.h"

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

Print * PersistentLogImpl_SD::open()
{
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

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
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
		printDate(mFile, dt);
		mFile.print(F(", "));
	}

	return &mFile;
}

void PersistentLogImpl_SD::close()
{
	mFile.close();
}
