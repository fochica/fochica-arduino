// GenericBLEModuleClient.h

#ifndef _GENERICBLEMODULECLIENT_h
#define _GENERICBLEMODULECLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial.h>
#include "IClientDevice.h"
#include "PacketType.h"
#include "PacketHeader.h"

// Implements a client using a HM-10 BLE module, or a clone.
// Supports any BLE module with a single characteristic.
// Serializes and deserializes the packets.
class GenericBLEModuleClient : public IClientDevice
{
public:
	GenericBLEModuleClient(int rxPin, int txPin, int sensePin);

	// sending packets
	bool sendTime(const PacketTime& packet);
	bool sendTechnicalData(const PacketTechnicalData& packet);
	bool sendSensorData(const PacketSensorData& packet);
	bool sendLogicalData(const PacketLogicalData& packet);
	bool sendCalibrationParams(const PacketCalibrationParams& packet);

	// receiving logic
	void work();

	// state
	void begin();
	bool isConnected();

private:
	SoftwareSerial mBLE;
	int mSensePin;
	bool mConnected; // last known connection state. true for connected.

	const int DELAY_AFTER_PACKET_SENT = 100; // ms
	const int PACKET_RECEIVE_TIMEOUT = 10; // ms
	const int BAUD_RATE = 9600; // default baud rate for HM-10. This is not a high throughput link, so keep slow for lower performance impact
	const int MAX_BLE_PACKET_LENGTH = 20; // max due to BLE restrictions

	const byte PROTOCOL_VERSION = 'a';
	const int HEADER_LENGTH = sizeof(PacketHeader);
	const int MIN_PACKET_LENGTH = HEADER_LENGTH+1; // min due to our protocol definition (header size)

	bool writePacket(PacketType::e type, const byte * buf, byte size);
	// receiving logic
	bool processIncomingIfAvailable();
	void flushIncomingBuffer(); // if we have issues parsing incoming message, flush the stream
	int getPacketLength(PacketType::e type);
};

#endif

