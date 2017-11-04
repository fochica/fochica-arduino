/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ESP32BLEModule.h

#ifndef _ESP32BLEMODULE_h
#define _ESP32BLEMODULE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef ESP32 // this implementation is ESP32 specific

#include "esp_gatts_api.h"

#include "ClientDevice.h"
#include "PacketType.h"
#include "PacketHeader.h"

///Attributes State Machine
enum {
	SPP_IDX_SVC,

	SPP_IDX_SPP_DATA_RECV_CHAR,
	SPP_IDX_SPP_DATA_RECV_VAL,

	SPP_IDX_SPP_DATA_NOTIFY_CHAR,
	SPP_IDX_SPP_DATA_NTY_VAL,
	SPP_IDX_SPP_DATA_NTF_CFG,

	SPP_IDX_NB,
};

// Implements an ESP32 BLE functionality.
// Exposes UART characteristic pair to be compatible with high-level BLE devices.
// Serializes and deserializes the packets.
// based on https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/ble_spp_server/main/ble_spp_server_demo.c
class ESP32BLEModule
{
public:
	static ESP32BLEModule& getInstance() {
		static ESP32BLEModule singleton;
		return singleton;
	}

	static const uint16_t MAX_CLIENTS = 4; // max number of concurent connections we support

	// interface for module clients to call back
	bool isConnected(uint16_t connId);
	bool sendPacket(uint16_t connId, PacketType::e type, const byte * buf, uint16_t size);

	// return individual connections to system
	ClientDevice * getClient(uint16_t connId);
protected:

private:
	// protect constructors to prevent singleton modification
	ESP32BLEModule();
	ESP32BLEModule(const ESP32BLEModule &rhs);
	ESP32BLEModule & operator=(const ESP32BLEModule &rhs);
	~ESP32BLEModule();

	friend void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

	typedef struct {
		bool isConnected;
		uint16_t mtu;
		bool enableNotification;
		ClientDevice * client;
	} clientDevice_t;

	// state
	esp_gatt_if_t mGattsIf; // gatts id, we only have one app, so should have only one id
	uint16_t mHandleTable[SPP_IDX_NB]; // Handles list
	clientDevice_t mClients[MAX_CLIENTS];
	int mConnectedClientCount;

	void initClient(clientDevice_t & c, uint16_t connId);
	uint8_t findCharAndDesrIndex(uint16_t handle);
	bool processIncomingPacket(uint16_t connId, uint8_t * buf, uint16_t size);
	void onClientConnectionChange(uint16_t connId, bool isConnected);

	static const byte PROTOCOL_VERSION = 'a';
	static const int HEADER_LENGTH = sizeof(PacketHeader);
	static const int MIN_PACKET_LENGTH = HEADER_LENGTH + 1; // min due to our protocol definition (header size)
	static const int MAX_BLE_PACKET_LENGTH = 20; // max due to BLE default mtu. we can increase mtu, but our data is below this to be compatible with other modules

	byte mTempPacketBuffer[MAX_BLE_PACKET_LENGTH]; // internal buffer to avoid allocations
	PacketHeader mTempPacketHeader;
};

#endif // ESP32

#endif
