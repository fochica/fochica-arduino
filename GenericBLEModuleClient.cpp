// 
// 
// 

#include "GenericBLEModuleClient.h"
#include "DebugStream.h"

GenericBLEModuleClient::GenericBLEModuleClient(int rxPin, int txPin, int sensePin) : mBLE(rxPin, txPin)
{
	mSensePin = sensePin;
}

bool GenericBLEModuleClient::sendTime(const PacketTime & packet)
{
	return writePacket(PacketType::Time, (const byte *)&packet, sizeof(PacketTime));
}

bool GenericBLEModuleClient::processIncomingIfAvailable()
{
	if (mBLE.available() == false)
		return false;

	byte length=mBLE.peek();
	if (length<MIN_PACKET_LENGTH || length>MAX_BLE_PACKET_LENGTH) {
		DebugStream->println("Invalid data on BLE receiver");
		return false;
	}

	byte payloadLength = length - HEADER_LENGTH;
	
	// read protocol version and verify

	// read packet type and branch

	// think how to read data, validate expected size, etc
	// once have data

	// temp:
	PacketTime packet;
	if (mServerCallback)
		mServerCallback->receiveTime(packet);
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

	return true;
}
