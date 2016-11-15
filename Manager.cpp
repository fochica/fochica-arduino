// 
// 
// 

#include "Manager.h"
#include "DebugStream.h"

void Manager::setRTC(IRTC * rtc)
{
	mRTC = rtc;
	if (DebugStream != NULL) {
		DebugStream->print(F("Time at RTC: "));
		PrintDate(*DebugStream, mRTC->getUnixTime());
		DebugStream->println();
	}
}

void Manager::work()
{
	mClientManager.work();
	mSensorManager.work();
	// TODO, send tech packet
}

bool Manager::receiveTime(const PacketTime & packet)
{
	if (DebugStream != NULL) {
		DebugStream->print(F("Got utc time "));
		DebugStream->println(packet.utcTime);
		DebugStream->print(F("Got time offset "));
		DebugStream->println(packet.offsetToLocal);
	}

	if (mRTC == NULL)
		return false; // we just ignore it, RTC is not mandatory

	mTimeOffsetFromUtcToLocal = packet.offsetToLocal;
	mRTC->setUnixTime(packet.utcTime + packet.offsetToLocal);
	
	// verify
	if (DebugStream!=NULL) {
		DebugStream->print(F("Value at RTC (with offset) "));
		DebugStream->println(mRTC->getUnixTime());
		// render as date parts and print
		PrintDate(*DebugStream, mRTC->getUnixTime());
		DebugStream->println();
	}

	// send new time packet to client, to update them of our new time
	sendTime();
	return true;
}

bool Manager::sendTime()
{
	if (mRTC == NULL) {
		if (DebugStream != NULL) DebugStream->println(F("Manager::sendTime, no RTC defined "));
		return false;
	}
	PacketTime packet;
	packet.utcTime = mRTC->getUnixTime() - mTimeOffsetFromUtcToLocal;
	packet.offsetToLocal = mTimeOffsetFromUtcToLocal;	
	return getClientManager().sendTime(packet);
}

void Manager::PrintDate(Print & out, const DateTime & d)
{
	out.print(d.year(), DEC);
	out.print('/');
	out.print(d.month(), DEC);
	out.print('/');
	out.print(d.day(), DEC);
	out.print(' ');
	out.print(d.hour(), DEC);
	out.print(':');
	out.print(d.minute(), DEC);
	out.print(':');
	out.print(d.second(), DEC);
}

