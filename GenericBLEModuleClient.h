/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// GenericBLEModuleClient.h

#ifndef _GENERICBLEMODULECLIENT_h
#define _GENERICBLEMODULECLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef __AVR__ // default SoftwareSerial is AVR specific, some other platforms have compatible implementations which can be included here for those platforms
#include <SoftwareSerial.h>
#define HAVE_SOFTWARE_SERIAL
#endif
#include "IClientDevice.h"
#include "PacketType.h"
#include "PacketHeader.h"

// Implements a client using a HM-10 BLE module, or a clone (CC41).
// Supports any BLE module with a single characteristic.
// Serializes and deserializes the packets.
class GenericBLEModuleClient : public IClientDevice
{
public:
#ifdef HAVE_SOFTWARE_SERIAL
	GenericBLEModuleClient(SoftwareSerial & serial, uint8_t statePin);
#endif
	GenericBLEModuleClient(HardwareSerial & serial, uint8_t statePin);

	// sending packets
	bool sendTime(const PacketTime& packet);
	bool sendTechnicalData(const PacketTechnicalData& packet);
	bool sendSensorData(const PacketSensorData& packet);
	bool sendLogicalData(const PacketLogicalData& packet);
	bool sendCalibrationParams(const PacketCalibrationParams& packet);

	// receiving logic
	void work();
	bool isCanReceivePackets();
	bool isListenLimited(); // does this device belong to a class where only one can listen at a time?
	void listen();

	// state
	void begin();
	bool isConnected();

private:
	Stream & mBLE;
	bool mIsSoftwareSerial;
	uint8_t mStatePin;
	bool mConnected; // last known connection state. true for connected.
	unsigned long mLastSendTime;

	const int DELAY_AFTER_PACKET_SENT = 30; // ms, how long to wait between sending two packets
	const int PACKET_RECEIVE_TIMEOUT = 10; // ms, what is considered a timeout when waiting for parts of a single packet
	const long BAUD_RATE = 9600; // default baud rate for HM-10/CC41. This is not a high throughput link, so keep slow for lower performance impact
	const int MAX_BLE_PACKET_LENGTH = 20; // max due to BLE restrictions

	const byte PROTOCOL_VERSION = 'a';
	const int HEADER_LENGTH = sizeof(PacketHeader);
	const int MIN_PACKET_LENGTH = HEADER_LENGTH+1; // min due to our protocol definition (header size)

	// sending logic
	bool writePacket(PacketType::e type, const byte * buf, byte size);
	// receiving logic
	bool processIncomingIfAvailable();
	void flushIncomingBuffer(); // if we have issues parsing incoming message, flush the stream
	int getPacketLength(PacketType::e type);
};

#endif

