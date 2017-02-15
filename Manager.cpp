// 
// 
// 

#include "Manager.h"
#include "DebugStream.h"
#include "RNGUtils.h"
#include "SoundManager.h"
#include "SeatOperation.h"
#include "SensorOperation.h"

Manager::Manager() : mSensorManager(mClientManager, *this)
{
	mDeviceUniqueId = 0; // mark as not initialized
	mDoneInitialSending = false;

	mRepeatSendLogicalData = 0;
	mRepeatSendCalibrationParams = 0;

	mEventHandlerAddedCount = mEventHandlerCount = 0;
	mEventHandlers = NULL;
}

Manager::~Manager()
{
	releaseEventHandlers();
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
	// handle sending repeats
	if (mRepeatSendLogicalData > 0) {
		sendLogicalDataOnce();
		mRepeatSendLogicalData--;
	}
	if (mRepeatSendCalibrationParams > 0) {
		sendCalibrationParamsOnce();
		mRepeatSendCalibrationParams--;
	}

	// sub modules
	mClientManager.work();
	mSensorManager.work();
	mTechnicalManager.work(&mClientManager);

	// event handlers
	eventHandlersWork();

	// handle sending of initial data to client on start, further sync will happen on events
	// https://trello.com/c/RDiZhuAY/81-send-packets-on-begin-if-connected
	/*
	if (mDoneInitialSending == false) {
		if (DebugStream != NULL) {
			DebugStream->println(F("Performing Initial Sending"));
		}

		if (mClientManager.isConnected()) {
			sendLogicalData();
			sendCalibrationParams();
		}
		mDoneInitialSending = true; // do only once
	}
	*/
}

// get notified of a connect or disconnect on one of the modules/adapters
// note that a disconnect in one adapter doesn't mean all adpaters are disconnected
void Manager::onClientConnectionChange(bool isConnected)
{
	if (DebugStream != NULL) {
		DebugStream->print(F("onClientConnectionChange, connected="));
		DebugStream->println(isConnected);
	}

	// some adapters belong to a class that shares listen rights
	mClientManager.reassignListenRight(); // reassign right, so that one that is connected is listenning. do this before sending logical data.

	// some data should be sent on connect/disconnect
	// it is ok to send through the clientManager. system should be robust if some packets are resent even if not needed

	// send logical data, this includes the number of connected adapters, so we need to resend on both the connect and disconnect (for sake of other adapters)
	sendLogicalData();

	if (isConnected) { // only send on connect, this is for data that doesn't change on disconnects
		// send calibration params
		sendCalibrationParams();
	}

	// update event handlers
	eventClientConnectionChange(isConnected);
}

bool Manager::receiveTime(const PacketTime & packet)
{
	if (DebugStream != NULL) {
		DebugStream->print(F("Got utc time: "));
		DebugStream->println(packet.utcTime);
		DebugStream->print(F("Got time offset: "));
		DebugStream->println(packet.offsetToLocal);
	}

	if (mRTC == NULL)
		return false; // we just ignore it, RTC is not mandatory

	// set RTC to correct time
	mTimeOffsetFromUtcToLocal = packet.offsetToLocal;
	if (DebugStream != NULL) {
		DebugStream->print(F("Setting RTC to: "));
		DebugStream->println(packet.utcTime + packet.offsetToLocal);
	}
	mRTC->setUnixTime(packet.utcTime + packet.offsetToLocal);
	
	// verify
	if (DebugStream!=NULL) {
		DebugStream->print(F("Value at RTC (with offset): "));
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

bool Manager::receiveSensorOperation(const PacketSensorOperation & packet)
{
	if (DebugStream != NULL) {
		DebugStream->print(F("Got sensor id "));
		DebugStream->println(packet.sensorId);
		DebugStream->print(F("Got sensor operation "));
		DebugStream->println(packet.operationId);
	}

	switch (packet.operationId)
	{
	case SensorOperation::SetActivityModeActive:
		getSensorManager().setSensorActivityMode(packet.sensorId, SensorActivityMode::Active);
		return true;
	case SensorOperation::SetActivityModeDeactivated:
		getSensorManager().setSensorActivityMode(packet.sensorId, SensorActivityMode::Deactivated);
		return true;
	case SensorOperation::SetActivityModeDisabled:
		getSensorManager().setSensorActivityMode(packet.sensorId, SensorActivityMode::Disabled);
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
	mRepeatSendLogicalData = PACKET_SEND_REPEATS;
	sendLogicalDataOnce();
	mRepeatSendLogicalData--;
}

bool Manager::sendLogicalDataOnce()
{
	if (DebugStream != NULL) {
		DebugStream->print(F("sendLogicalDataOnce, mRepeatSendLogicalData="));
		DebugStream->println(mRepeatSendLogicalData);
	}

	PacketLogicalData packet;
	packet.deviceUniqueId = getDeviceUniqueId();
	packet.clientCount = mClientManager.getDeviceCount();
	packet.connectedClientCount = mClientManager.getConnectedCount();
	packet.seatCount = mSensorManager.getSeatCount();
	packet.sensorCount = mSensorManager.getSensorAddedCount();
	packet.capabilityMask = 0;
	return getClientManager().sendLogicalData(packet);
}

bool Manager::sendCalibrationParams()
{
	mRepeatSendCalibrationParams = PACKET_SEND_REPEATS;
	sendCalibrationParamsOnce();
	mRepeatSendCalibrationParams--;
}

bool Manager::sendCalibrationParamsOnce()
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

void Manager::releaseEventHandlers()
{
	if (mEventHandlers != NULL) {
		delete[] mEventHandlers;
		mEventHandlers = NULL;
	}
}

void Manager::setEventHandlerCount(int count)
{
	releaseEventHandlers();
	mEventHandlerCount = count;
	mEventHandlerAddedCount = 0;
	mEventHandlers = new IEventHandler *[count];
}

bool Manager::addEventHandler(IEventHandler * handler)
{
	if (mEventHandlerAddedCount >= mEventHandlerCount) {
		if (DebugStream != NULL) DebugStream->println(F("Manager::addEventHandler, trying to add more events than declared"));
		return false;
	}

	mEventHandlers[mEventHandlerAddedCount] = handler;

	mEventHandlerAddedCount++;
	return true;
}

void Manager::eventHandlersWork()
{
	for (int i = 0; i < mEventHandlerAddedCount; i++)
		mEventHandlers[i]->work(*this);
}

bool Manager::eventSeatStateChange(seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	for (int i = 0; i < mEventHandlerAddedCount; i++) {
		//if (DebugStream) {
		//	DebugStream->print(F("Calling eventSeatStateChange on an event handler, seatId="));
		//	DebugStream->print(seatId);
		//	DebugStream->println();
		//}
		if (mEventHandlers[i]->eventSeatStateChange(*this, seatId, lastState, newState) == false)
			return false;
	}
	return true;
}

bool Manager::eventSensorStateChange(seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState)
{
	for (int i = 0; i < mEventHandlerAddedCount; i++) {
		if (mEventHandlers[i]->eventSensorStateChange(*this, seatId, sensorId, lastState, newState) == false)
			return false;
	}
	return true;
}

bool Manager::eventClientConnectionChange(bool isAdapterConnected)
{
	for (int i = 0; i < mEventHandlerAddedCount; i++) {
		if (mEventHandlers[i]->eventClientConnectionChange(*this, isAdapterConnected) == false)
			return false;
	}
	return true;
}

