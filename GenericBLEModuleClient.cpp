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

#include "GenericBLEModuleClient.h"
#include "DebugStream.h"

//#define DEBUG_LOG_PACKET_INFO

#ifndef __AVR_ATmega328P__ // Exclude debugging messages to fit program in limited flash of smaller AVRs
#define DEBUG_BLE_ERRORS
#endif

#ifdef HAVE_SOFTWARE_SERIAL
GenericBLEModuleClient::GenericBLEModuleClient(SoftwareSerial & serial, uint8_t statePin) : mBLE(serial)
{
	mStatePin = statePin;
	mConnected = false;
	mLastSendTime = 0;
	mIsSoftwareSerial = true;
}
#endif

GenericBLEModuleClient::GenericBLEModuleClient(HardwareSerial & serial, uint8_t statePin) : mBLE(serial)
{
	mStatePin = statePin;
	mConnected = false;
	mLastSendTime = 0;
	mIsSoftwareSerial = false;
}

void GenericBLEModuleClient::begin()
{
	// commands like pinMode should go in setup and not at a point where global constructors are called
	// http://forum.arduino.cc/index.php?topic=212844.0
	pinMode(mStatePin, INPUT);
	if (mIsSoftwareSerial) {
#ifdef HAVE_SOFTWARE_SERIAL
		SoftwareSerial & ss((SoftwareSerial &)mBLE);
		ss.begin(BAUD_RATE);
#endif
	}
	else {
		HardwareSerial & hs((HardwareSerial &)mBLE);
		hs.begin(BAUD_RATE);
	}
	mBLE.setTimeout(PACKET_RECEIVE_TIMEOUT);
}

bool GenericBLEModuleClient::isConnected()
{
	return digitalRead(mStatePin);
}

bool GenericBLEModuleClient::writePacket(PacketType::e type, const byte * buf, byte size)
{
#ifdef DEBUG_LOG_PACKET_INFO
	if (DebugStream) {
		DebugStream->print(F("writePacket "));
		DebugStream->print(mStatePin); // to identify the module
		DebugStream->print(F(", "));
		DebugStream->println((char)type);
	}
#endif

	if (!isConnected()) // check if we are connected, otherwise we will send packet data in config mode (and not data/comm mode)
		return false;

	// wait for previous data to flush so it considered as one whole packet for BLE modules.
	// don't just do "delay(DELAY_AFTER_PACKET_SENT);" after sending,
	// instead of a fixed delay every time, store time last sent and delay as little as needed to have DELAY_AFTER_PACKET_SENT since then
	unsigned long now = millis();
	unsigned long passed = now - mLastSendTime;
	if (now < mLastSendTime) // if time wrapped
		passed = 0; // force delay
	if (passed < DELAY_AFTER_PACKET_SENT)
		delay(DELAY_AFTER_PACKET_SENT - passed);
	mLastSendTime = millis(); // store correct time, after possible delay

	// write length. length is 3 header bytes and the payload
	byte len = size + HEADER_LENGTH;
	if (len > MAX_BLE_PACKET_LENGTH)
		return false;
	mBLE.write(len);
	// write protocol version, this is for our forward compatibility
	mBLE.write(PROTOCOL_VERSION);
	// write packet type
	mBLE.write((byte)type);
	// write the data
	mBLE.write(buf, size);

#ifdef DEBUG_LOG_PACKET_INFO
	/*
	printf("write packet, len %d, ver %d, type %d, data size %d, passed %d,  ", len, PROTOCOL_VERSION, type, size, passed);
	for (int i = 0; i < size; i++)
		printf("%2X ", buf[i]);
	printf("\n");
	*/
#endif

	return true;
}

void GenericBLEModuleClient::flushIncomingBuffer()
{
	// read from input until we timeout
	// assume timeout means packet boundaries
	byte x;
	if (DebugStream != NULL) DebugStream->print(F("Flushing inbuf: "));
	while (mBLE.readBytes(&x, 1) == 1) { // on timeout we will get 0
		if (DebugStream != NULL) DebugStream->print(x);
	}
	if (DebugStream != NULL) DebugStream->println();
}

bool GenericBLEModuleClient::sendTime(const PacketTime & packet)
{
	return writePacket(PacketType::Time, (const byte *)&packet, sizeof(PacketTime));
}

bool GenericBLEModuleClient::sendTechnicalData(const PacketTechnicalData& packet)
{
	return writePacket(PacketType::TechnicalData, (const byte *)&packet, sizeof(PacketTechnicalData));
}

bool GenericBLEModuleClient::sendSensorData(const PacketSensorData& packet)
{
	return writePacket(PacketType::SensorData, (const byte *)&packet, sizeof(PacketSensorData));
}

bool GenericBLEModuleClient::sendLogicalData(const PacketLogicalData & packet)
{
	return writePacket(PacketType::LogicalData, (const byte *)&packet, sizeof(PacketLogicalData));
}

bool GenericBLEModuleClient::sendCalibrationParams(const PacketCalibrationParams & packet)
{
	return writePacket(PacketType::CalibrationParams, (const byte *)&packet, sizeof(PacketCalibrationParams));
}

void GenericBLEModuleClient::work()
{
	// manage changes in connection state
	// connections and disconnections
	bool conn = isConnected();
	if (conn != mConnected) {
		if (mServerCallback)
			mServerCallback->onClientConnectionChange(this, conn); // notify about change
		mConnected = conn;
	}
	// process incoming data
	if (conn)
		processIncomingIfAvailable(); 

}

bool GenericBLEModuleClient::isCanReceivePackets()
{
#ifdef HAVE_SOFTWARE_SERIAL
	if (mIsSoftwareSerial) {
		SoftwareSerial & ss((SoftwareSerial &)mBLE);
		return ss.isListening(); // limitation of AVR's SoftwareSerial implementation
	}
#endif
	return true; // hardware serial is always ready to receive
}

bool GenericBLEModuleClient::isListenLimited()
{
	return mIsSoftwareSerial;
}

void GenericBLEModuleClient::listen()
{
#ifdef HAVE_SOFTWARE_SERIAL
	if (mIsSoftwareSerial) {
		SoftwareSerial & ss((SoftwareSerial &)mBLE);
		ss.listen();
	}
#endif
}

bool GenericBLEModuleClient::processIncomingIfAvailable()
{
#ifdef DEBUG_LOG_PACKET_INFO
	if (DebugStream) {
		DebugStream->print(F("processIncomingIfAvailable, pin: "));
		DebugStream->print(mStatePin);
		if (mIsSoftwareSerial) {
			SoftwareSerial & ss((SoftwareSerial &)mBLE);
			DebugStream->print(F(", listening: "));
			DebugStream->print(ss.isListening());
		}
		DebugStream->print(F(", available: "));
		DebugStream->println(mBLE.available());
	}
#endif

	if (mBLE.available() == false)
		return false;

	// peek at length
	byte length = mBLE.peek();
	if (length<MIN_PACKET_LENGTH || length>MAX_BLE_PACKET_LENGTH) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid data in BLE, l="));
			DebugStream->println(length);
		}
#endif
		flushIncomingBuffer();
		return false;
	}

	byte payloadLength = length - HEADER_LENGTH;

	// read header
	PacketHeader header;
	int count = mBLE.readBytes((uint8_t *)&header, sizeof(PacketHeader));
	if (count != sizeof(PacketHeader)) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Failure reading header in BLE, read="));
			DebugStream->println(count);
		}
#endif
		flushIncomingBuffer();
		return false;
	}
	if (header.length != length) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid header data in BLE, header.l="));
			DebugStream->print(header.length);
			DebugStream->print(F(", length="));
			DebugStream->println(length);
		}
#endif
		flushIncomingBuffer();
		return false;
	}

	// verify protocol version
	if (header.protocolVersion != PROTOCOL_VERSION) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid protocol version in BLE, expected="));
			DebugStream->print(PROTOCOL_VERSION);
			DebugStream->print(F(", received="));
			DebugStream->println(header.protocolVersion);
		}
#endif
		flushIncomingBuffer();
		return false;
	}

	// read packet type and validate expected size
	PacketType::e type = (PacketType::e)header.packetType;
	int expectedLength = IServer::getPacketLength(type);
	if (expectedLength == -1) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid packet type in BLE, type="));
			DebugStream->println(type);
		}
#endif
		flushIncomingBuffer(); // consider to just read expectedLength instead...
		return false;
	}
	if (payloadLength != expectedLength) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid data length for packet type in BLE, expected="));
			DebugStream->print(expectedLength);
			DebugStream->print(F(", available="));
			DebugStream->println(payloadLength);
		}
#endif
		flushIncomingBuffer(); // consider to just read expectedLength instead...
		return false;
	}

	// branch on packet types
	// assume expectedLength == length of a packet of type "type"
	switch (type) {
	case PacketType::Time:
		PacketTime packet;
		count = mBLE.readBytes((uint8_t *)&packet, expectedLength);
		if (mServerCallback && count==expectedLength)
			mServerCallback->receiveTime(this, packet);
		break;
	case PacketType::SeatOperation:
		PacketSeatOperation packetSO;
		count = mBLE.readBytes((uint8_t *)&packetSO, expectedLength);
		if (mServerCallback && count == expectedLength)
			mServerCallback->receiveSeatOperation(this, packetSO);
		break;
	case PacketType::SensorOperation:
		PacketSensorOperation packetSnO;
		count = mBLE.readBytes((uint8_t *)&packetSnO, expectedLength);
		if (mServerCallback && count == expectedLength)
			mServerCallback->receiveSensorOperation(this, packetSnO);
		break;
	default:
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Not handled packet type in BLE, type="));
			DebugStream->println(type);
		}
#endif
		flushIncomingBuffer();
		return false;
	}

	// handle error in reading
	if (count != expectedLength) {
#ifdef DEBUG_BLE_ERRORS
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid data received for packet type in BLE, type="));
			DebugStream->print(type);
			DebugStream->print(F(", read="));
			DebugStream->print(count);
			DebugStream->print(F(", expected="));
			DebugStream->println(expectedLength);
		}
#endif
		flushIncomingBuffer();
		return false;
	}

	return true; // got (and processed?) something
}
