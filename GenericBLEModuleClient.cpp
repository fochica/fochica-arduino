// 
// 
// 

#include "GenericBLEModuleClient.h"
#include "DebugStream.h"

GenericBLEModuleClient::GenericBLEModuleClient(int rxPin, int txPin, int statePin) : mBLE(rxPin, txPin)
{
	mStatePin = statePin;
	mConnected = false;
	mLastSendTime = 0;
}

void GenericBLEModuleClient::begin()
{
	// commands like pinMode should go in setup and not at a point where global constructors are called
	// http://forum.arduino.cc/index.php?topic=212844.0
	pinMode(mStatePin, INPUT);
	mBLE.begin(BAUD_RATE);
	mBLE.setTimeout(PACKET_RECEIVE_TIMEOUT);
}

bool GenericBLEModuleClient::isConnected()
{
	return digitalRead(mStatePin);
}

bool GenericBLEModuleClient::writePacket(PacketType::e type, const byte * buf, byte size)
{
	if (DebugStream) {
		DebugStream->print("writePacket ");
		DebugStream->print(mStatePin);
		DebugStream->print(", ");
		DebugStream->println((char)type);
	}

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

void GenericBLEModuleClient::work()
{
	// manage changes in connection state
	// connections and disconnections
	bool conn = isConnected();
	if (conn != mConnected) {
		if (mServerCallback)
			mServerCallback->onClientConnectionChange(conn); // notify about change
		mConnected = conn;
	}
	// process incoming data
	if (conn)
		processIncomingIfAvailable(); 

}

bool GenericBLEModuleClient::processIncomingIfAvailable()
{
	if (DebugStream) {
		DebugStream->print("processIncomingIfAvailable ");
		DebugStream->print(mStatePin);
		DebugStream->print(", ");
		DebugStream->println(mBLE.available());
	}

	if (mBLE.available() == false)
		return false;

	// peek at length
	byte length = mBLE.peek();
	if (length<MIN_PACKET_LENGTH || length>MAX_BLE_PACKET_LENGTH) {
		if (DebugStream != NULL) DebugStream->println(F("Invalid data in BLE receiver"));
		flushIncomingBuffer();
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
	case PacketType::SeatOperation:
		return sizeof(PacketSeatOperation);
	}
	return -1; // error, invalid or unknown packet type
}

