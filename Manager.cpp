// 
// 
// 

#include "Manager.h"
#include "DebugStream.h"
#include "RNGUtils.h"
#include "SoundManager.h"
#include "SeatOperation.h"

Manager::Manager() : mSensorManager(mClientManager)
{
	mDeviceUniqueId = 0; // mark as not initialized
}

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
	mTechnicalManager.work(&mClientManager);
}

void Manager::onClientConnectionChange(bool isConnected)
{
	// beep
	SoundManager::getInstance().playBeep(isConnected ? BeepType::ClientConnected : BeepType::ClientDisconnected);
	// some data should be sent on connect
	if (isConnected) {
		// it is ok to send through the clientManager. system should be robust if some packets are resent even if not needed
		// send logical data
		sendLogicalData();
		// send calibration params
		sendCalibrationParams();
	}
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

bool Manager::receiveSeatOperation(const PacketSeatOperation & packet)
{
	if (DebugStream != NULL) {
		DebugStream->print(F("Got seat id "));
		DebugStream->println(packet.seatId);
		DebugStream->print(F("Got seat operation "));
		DebugStream->println(packet.operationId);
	}

	switch (packet.operationId)
	{
	case SeatOperation::StartCalibrationOccupied:
		getSensorManager().calibrate(packet.seatId, SensorState::Occupied);
		return true;
	case SeatOperation::StartCalibrationEmpty:
		getSensorManager().calibrate(packet.seatId, SensorState::Empty);
		return true;
	default:
		break;
	}
	
	return false;
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

bool Manager::sendLogicalData()
{
	PacketLogicalData packet;
	packet.deviceUniqueId = getDeviceUniqueId();
	packet.clientCount = mClientManager.getDeviceCount();
	packet.connectedClientCount = mClientManager.getConnectedCount();
	packet.seatCount = mSensorManager.getSeatCount();
	packet.sensorCount = mSensorManager.getSensorAddedCount();
	return getClientManager().sendLogicalData(packet);
}

bool Manager::sendCalibrationParams()
{
	return getSensorManager().sendCalibrationParams();
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

unsigned long Manager::getDeviceUniqueId()
{
	if (mDeviceUniqueId != 0) // if already generated
		return mDeviceUniqueId;
	do
	{
		mDeviceUniqueId = RNGUtils::generateEntropyWithAnalogInputs();
	} while (mDeviceUniqueId == 0); // generate from entropy source until we get a value that is not 0
	return mDeviceUniqueId;
}

