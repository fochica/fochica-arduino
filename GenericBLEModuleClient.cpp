// 
// 
// 

#include "GenericBLEModuleClient.h"
#include "DebugStream.h"

GenericBLEModuleClient::GenericBLEModuleClient(int rxPin, int txPin, int sensePin) : mBLE(rxPin, txPin)
{
	mSensePin = sensePin;
}

void GenericBLEModuleClient::begin()
{
	// commands like pinMode should go in setup and not at a point where global cinstructors are called
	// http://forum.arduino.cc/index.php?topic=212844.0
	pinMode(mSensePin, INPUT);
	mBLE.begin(BAUD_RATE);
	mBLE.setTimeout(PACKET_RECEIVE_TIMEOUT);
}

bool GenericBLEModuleClient::isConnected()
{
	return digitalRead(mSensePin);
}

bool GenericBLEModuleClient::writePacket(PacketType::e type, const byte * buf, byte size)
{
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

	// wait for data to flush so it is one whole packet for BLE modules
	delay(DELAY_AFTER_PACKET_SENT);

	return true;
}

void GenericBLEModuleClient::flushIncomingBuffer()
{
	// read from input until we timeout
	// assume timeout means packet boundaries
	byte x;
	while (mBLE.readBytes(&x,1) == 1); // on timeout we will get 0
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

bool GenericBLEModuleClient::processIncomingIfAvailable()
{
	if (mBLE.available() == false)
		return false;

	// peek at length
	byte length = mBLE.peek();
	if (length<MIN_PACKET_LENGTH || length>MAX_BLE_PACKET_LENGTH) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid data in BLE receiver"));
		return false;
	}

	byte payloadLength = length - HEADER_LENGTH;

	// read header
	PacketHeader header;
	int count = mBLE.readBytes((uint8_t *)&header, sizeof(PacketHeader));
	if (count != sizeof(PacketHeader) || header.length != length) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid header data in BLE receiver"));
		flushIncomingBuffer();
		return false;
	}

	// verify protocol version
	if (header.protocolVersion != PROTOCOL_VERSION) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid protocol version in BLE receiver"));
		flushIncomingBuffer();
		return false;
	}

	// read packet type and validate expected size
	PacketType::e type = (PacketType::e)header.packetType;
	int expectedLength = getPacketLength(type);
	if (expectedLength == -1) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid packet type in BLE receiver"));
		flushIncomingBuffer(); // consider to just read expectedLength instead...
		return false;
	}
	if (payloadLength != expectedLength) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid data length for packet type in BLE receiver"));
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
			mServerCallback->receiveTime(packet);
		break;
	case PacketType::SeatOperation:
		PacketSeatOperation packetSO;
		count = mBLE.readBytes((uint8_t *)&packet, expectedLength);
		if (mServerCallback && count == expectedLength)
			mServerCallback->receiveSeatOperation(packetSO);
		break;
	default:
		if (DebugStream != NULL) DebugStream->println(F("Unknown packet type in BLE receiver"));
		flushIncomingBuffer();
		return false;
	}

	// handle error in reading
	if (count != expectedLength) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid data received for packet type in BLE receiver"));
		flushIncomingBuffer();
		return false;
	}

	return true; // got (and processed?) something
}

int GenericBLEModuleClient::getPacketLength(PacketType::e type)
{
	switch (type) {
	case PacketType::Time:
		return sizeof(PacketTime);
	}
	return -1; // error, invalid or unknown packet type
}

