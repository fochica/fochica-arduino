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

#ifdef ESP32 // this implementation is ESP32 specific

#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"

#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h" // for getting MAC address

#include "ESP32BLEModule.h"
#include "ESP32BLEModuleClient.h"
#include "DebugStream.h"

#define BLE_DEFAULT_MTU 23
#define BLE_DATA_MAX_LEN           (512)
#define BLE_DATA_OVERHEAD 3

#define BT_BD_ADDR_STR              "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr) addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]

#define TAG  "BLE" // TAG for logs

#define APP_ID              0x56 // some app id const for esp_ble_gatts_app_register
#define DEVICE_NAME          "ESP32_v2" // device name for use with esp_ble_gap_set_device_name
#define SPP_SVC_INST_ID	            0 // service instance id for use with esp_ble_gatts_create_attr_tab

// UUIDS
// https://mcdaniel.ws/Utility/Guidgen

/// SPP Service
//static const uint16_t spp_service_uuid = 0xABF0; // original UUID, 0000ABF0-0000-1000-8000-00805F9B34FB
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // Nordic UART service UUID
static uint8_t spp_service_uuid_full[16] = {
	/* LSB <--------------------------------------------------> MSB */
	//16 bytes
	0x9e,0xca,0xdc,0x24,0x0e,0xe5, 0xa9,0xe0, 0x93,0xf3, 0xa3,0xb5, 0x01,0x00,0x40,0x6e
};

/// Characteristic UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
//static const uint16_t spp_data_receive_uuid = 0xABF1;
//static const uint16_t spp_data_notify_uuid = 0xABF2;
static uint8_t spp_data_receive_uuid_full[16] = {
	0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e
};
static uint8_t spp_data_notify_uuid_full[16] = {
	0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e
};

static esp_ble_adv_data_t spp_adv_data = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = false,
	.min_interval = 0, //
	.max_interval = 0, //
	.appearance = 0x00,
	.manufacturer_len = 0,
	.p_manufacturer_data = 0,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = sizeof(spp_service_uuid_full),
	.p_service_uuid = spp_service_uuid_full,
	.flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
};
// advertizement parameters
static esp_ble_adv_params_t spp_adv_params = {
	.adv_int_min = 0x20, // ~ 20ms
	.adv_int_max = 0xA0, // ~ 100ms
	.adv_type = ADV_TYPE_IND,
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.peer_addr = { 0,0,0,0,0,0 },
	.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


/*
*  SPP PROFILE ATTRIBUTES
****************************************************************************************
*/

#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE_NR;

///SPP Service - data receive characteristic, write without response
static const uint8_t  spp_data_receive_val[20] = { 0x00 };

///SPP Service - data notify characteristic, notify
static const uint8_t  spp_data_notify_val[20] = { 0x00 };
static const uint8_t  spp_data_notify_ccc[2] = { 0x00, 0x00 }; // ENABLE_NOTIFICATION_VALUE = {0x01, 0x00}, ENABLE_INDICATION_VALUE = {0x02, 0x00}, DISABLE_NOTIFICATION_VALUE = {0x00, 0x00}

///Full HRS Database Description - Used to add attributes into the database
static const esp_gatts_attr_db_t spp_gatt_db[SPP_IDX_NB] =
{
	//SPP -  Service Declaration
	[SPP_IDX_SVC] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
	sizeof(spp_service_uuid_full), sizeof(spp_service_uuid_full), (uint8_t *)&spp_service_uuid_full } },

	// WRITE characteristic for receive
	//SPP -  data receive characteristic Declaration
	[SPP_IDX_SPP_DATA_RECV_CHAR] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
	CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write } },

	//SPP -  data receive characteristic Value
	[SPP_IDX_SPP_DATA_RECV_VAL] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_128, (uint8_t *)&spp_data_receive_uuid_full, ESP_GATT_PERM_WRITE,
	BLE_DATA_MAX_LEN,sizeof(spp_data_receive_val), (uint8_t *)spp_data_receive_val } },

	// NOTIFY characteristic for send
	//SPP -  data notify characteristic Declaration
	[SPP_IDX_SPP_DATA_NOTIFY_CHAR] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
	CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_notify } },

	//SPP -  data notify characteristic Value
	[SPP_IDX_SPP_DATA_NTY_VAL] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_128, (uint8_t *)&spp_data_notify_uuid_full, 0,
	BLE_DATA_MAX_LEN, sizeof(spp_data_notify_val), (uint8_t *)spp_data_notify_val } },

	//SPP -  data notify characteristic - Client Characteristic Configuration Descriptor
	[SPP_IDX_SPP_DATA_NTF_CFG] =
	{ { ESP_GATT_AUTO_RSP },{ ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
	sizeof(uint16_t),sizeof(spp_data_notify_ccc), (uint8_t *)spp_data_notify_ccc } },
};

/// Functions
// declare at top
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// Singleton contructor and BLE initialization
ESP32BLEModule::ESP32BLEModule()
{
	mGattsIf = ESP_GATT_IF_NONE;
	mConnectedClientCount = 0;

	// init clients
	for (int i = 0; i < MAX_CLIENTS; i++) {
		initClient(mClients[i], i);
		mClients[i].client = new ESP32BLEModuleClient(*this, i); // this is a singleton and we allocate once, not critical to release
	}

	// init BLE system
	esp_err_t ret;

	// Initialize NVS — it is used to store PHY calibration data
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// release BT classic memory
	ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
	if (ret) {
		ESP_LOGI(tag, "Bluetooth controller release classic bt memory failed");
		return;
	}

	// init bt
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	if (ret) {
		ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
		return;
	}

	ret = esp_bt_controller_enable(/*ESP_BT_MODE_BTDM*/ESP_BT_MODE_BLE);
	if (ret) {
		ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
		return;
	}

	ESP_LOGI(TAG, "%s init bluetooth\n", __func__);
	ret = esp_bluedroid_init();
	if (ret) {
		ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
		return;
	}
	ret = esp_bluedroid_enable();
	if (ret) {
		ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
		return;
	}

	// now can check on our MAC address
	const uint8_t *mac = esp_bt_dev_get_address();
	ESP_LOGI(TAG, "Bluetooth MAC: " BT_BD_ADDR_STR "\n", BT_BD_ADDR_HEX(mac));

	esp_ble_gatts_register_callback(gatts_event_handler);
	esp_ble_gap_register_callback(gap_event_handler);
	esp_ble_gatts_app_register(APP_ID); // now we wait for success events
	configSecurity();
}

// from https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/gatt_security_server/main/example_ble_sec_gatts_demo.c
// BLE security summary at https://eewiki.net/display/Wireless/A+Basic+Introduction+to+BLE+Security
// we will declare no IO capabilities and this will result in a Just WorksTM mode. Good security for BT 4.2 and up. Less strong for older versions. Assume pairing step is vulnerable against MitM, perform in a secure location.
void ESP32BLEModule::configSecurity()
{
	/* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;     //bonding with peer device after authentication
	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
	uint8_t key_size = 16;      //the key size should be 7~16 bytes
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
	/* If your BLE device act as a Slave, the init_key means you hope which types of key of the master should distribut to you,
	and the response key means which key you can distribut to the Master;
	If your BLE device act as a master, the response key means you hope which types of key of the slave should distribut to you,
	and the init key means which key you can distribut to the slave. */
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

ESP32BLEModule::~ESP32BLEModule()
{
}

void ESP32BLEModule::initClient(clientDevice_t & c, uint16_t id) {
	c.isConnected = false;
	c.mtu = BLE_DEFAULT_MTU;
	c.enableNotification = false;
}

// helper to map handle to our simple enum id
uint8_t ESP32BLEModule::findCharAndDesrIndex(uint16_t handle)
{
	uint8_t error = 0xff;

	for (int i = 0; i < SPP_IDX_NB; i++) {
		if (handle == mHandleTable[i]) {
			return i;
		}
	}

	return error;
}

bool ESP32BLEModule::processIncomingPacket(uint16_t connId, uint8_t * buf, uint16_t size)
{
	// get access to client connection
	ESP32BLEModuleClient * client = (ESP32BLEModuleClient *)(mClients[connId].client);
	IServer * server = client->getServerCallback();
	//printf("processIncomingPacket client cast %d, client %d, connId %d, server %d\n", client, mClients[connId].client, connId, server);
	if (server == NULL) {
		if (DebugStream != NULL) {
			DebugStream->println(F("Invalid server callback in BLE"));
		}
		return false; // why try if nowhere to send result
	}

	// parse data

	// verify size
	if (size<MIN_PACKET_LENGTH || size>MAX_BLE_PACKET_LENGTH) {
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid data in BLE, size="));
			DebugStream->println(size);
		}
		return false;
	}

	// read header
	memcpy(&mTempPacketHeader, buf, HEADER_LENGTH);
	if (size!=mTempPacketHeader.length) {
		if (DebugStream != NULL) {
			DebugStream->print(F("Length mismatch in BLE, size="));
			DebugStream->print(size);
			DebugStream->print(F(", length="));
			DebugStream->println(mTempPacketHeader.length);
		}
		return false;
	}

	byte payloadLength = mTempPacketHeader.length - HEADER_LENGTH;

	// verify protocol version
	if (mTempPacketHeader.protocolVersion != PROTOCOL_VERSION) {
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid protocol version in BLE, expected="));
			DebugStream->print(PROTOCOL_VERSION);
			DebugStream->print(F(", received="));
			DebugStream->println(mTempPacketHeader.protocolVersion);
		}
		return false;
	}

	// read packet type and validate expected size
	PacketType::e type = (PacketType::e)mTempPacketHeader.packetType;
	int expectedLength = IServer::getPacketLength(type);
	if (expectedLength == -1) {
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid packet type in BLE, type="));
			DebugStream->println(type);
		}
		return false;
	}
	if (payloadLength != expectedLength) {
		if (DebugStream != NULL) {
			DebugStream->print(F("Invalid data length for packet type in BLE, expected="));
			DebugStream->print(expectedLength);
			DebugStream->print(F(", available="));
			DebugStream->println(payloadLength);
		}
		return false;
	}

	// branch on packet types
	// assume expectedLength == length of a packet of type "type"
	switch (type) {
	case PacketType::Time:
		PacketTime packet;
		memcpy((uint8_t *)&packet, buf+HEADER_LENGTH, expectedLength);
		server->receiveTime(client, packet);
		break;
	case PacketType::SeatOperation:
		PacketSeatOperation packetSO;
		memcpy((uint8_t *)&packetSO, buf + HEADER_LENGTH, expectedLength);
		server->receiveSeatOperation(client, packetSO);
		break;
	case PacketType::SensorOperation:
		PacketSensorOperation packetSnO;
		memcpy((uint8_t *)&packetSnO, buf + HEADER_LENGTH, expectedLength);
		server->receiveSensorOperation(client, packetSnO);
		break;
	default:
		if (DebugStream != NULL) {
			DebugStream->print(F("Not handled packet type in BLE, type="));
			DebugStream->println(type);
		}
		return false;
	}

	return true; // got (and processed?) something

}

void ESP32BLEModule::onClientConnectionChange(uint16_t connId, bool isConnected)
{
	ESP32BLEModuleClient * client = (ESP32BLEModuleClient *)(mClients[connId].client);
	client->getServerCallback()->onClientConnectionChange(client, false);
}

bool ESP32BLEModule::isConnected(uint16_t connId)
{
	if(connId>=MAX_CLIENTS)
		return false;
	return mClients[connId].isConnected && mClients[connId].enableNotification; // consider connected only if notifications are enabled, so that we don't start sending before the other side is listening
}

bool ESP32BLEModule::sendPacket(uint16_t connId, PacketType::e type, const byte * buf, uint16_t size)
{
	clientDevice_t & c = mClients[connId];
	printf("connId %d, connected %d, notify %d, mtu %d, packet type %d, size %d\n", connId, c.isConnected, c.enableNotification, c.mtu, type, size);
	if (c.isConnected && c.enableNotification) {
		if (size <= (c.mtu - BLE_DATA_OVERHEAD)) {
			// serialize full packet with header
			byte len = size + HEADER_LENGTH;
			mTempPacketBuffer[0] = len; // might be more correct to populate header structure and then copy it
			mTempPacketBuffer[1] = PROTOCOL_VERSION;
			mTempPacketBuffer[2] = type;
			memcpy(mTempPacketBuffer + HEADER_LENGTH, buf, size);
			// send
			esp_err_t ret=esp_ble_gatts_send_indicate(mGattsIf, connId, mHandleTable[SPP_IDX_SPP_DATA_NTY_VAL], len, mTempPacketBuffer, false);
			if (ret != ESP_OK) {
				printf("Error sending notification %d\n", ret);
			}
		}
		else {
			printf("Can't notify, too long\n");
			return false;
		}
	}
	else {
		return false;
	}
	
	return true;
}

ClientDevice * ESP32BLEModule::getClient(uint16_t connId)
{
	if (connId >= MAX_CLIENTS)
		return NULL;
	return mClients[connId].client;
}

/////////////////////////////
// Event handlers

// debug helping list
const char *esp_gap_ble_cb_event_text[] = {
	"ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_RESULT_EVT",
	"ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT",
	"ESP_GAP_BLE_ADV_START_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_START_COMPLETE_EVT",
	"ESP_GAP_BLE_AUTH_CMPL_EVT",
	"ESP_GAP_BLE_KEY_EVT",
	"ESP_GAP_BLE_SEC_REQ_EVT",
	"ESP_GAP_BLE_PASSKEY_NOTIF_EVT",
	"ESP_GAP_BLE_PASSKEY_REQ_EVT",
	"ESP_GAP_BLE_OOB_REQ_EVT",
	"ESP_GAP_BLE_LOCAL_IR_EVT",
	"ESP_GAP_BLE_LOCAL_ER_EVT",
	"ESP_GAP_BLE_NC_REQ_EVT",
	"ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT",
	"ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT",
	"ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT",
	"ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT",
	"ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT",
	"ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT",
	"ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT",
	"ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT",
	"ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT",
	"ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT",
	"ESP_GAP_BLE_ADD_WHITELIST_COMPLETE_EVT",
	"ESP_GAP_BLE_EVT_MAX"
};

static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
	char *key_str = NULL;
	switch (key_type) {
	case ESP_LE_KEY_NONE:
		key_str = "ESP_LE_KEY_NONE";
		break;
	case ESP_LE_KEY_PENC:
		key_str = "ESP_LE_KEY_PENC";
		break;
	case ESP_LE_KEY_PID:
		key_str = "ESP_LE_KEY_PID";
		break;
	case ESP_LE_KEY_PCSRK:
		key_str = "ESP_LE_KEY_PCSRK";
		break;
	case ESP_LE_KEY_PLK:
		key_str = "ESP_LE_KEY_PLK";
		break;
	case ESP_LE_KEY_LLK:
		key_str = "ESP_LE_KEY_LLK";
		break;
	case ESP_LE_KEY_LENC:
		key_str = "ESP_LE_KEY_LENC";
		break;
	case ESP_LE_KEY_LID:
		key_str = "ESP_LE_KEY_LID";
		break;
	case ESP_LE_KEY_LCSRK:
		key_str = "ESP_LE_KEY_LCSRK";
		break;
	default:
		key_str = "INVALID BLE KEY TYPE";
		break;
	}
	return key_str;
}

void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	ESP_LOGI(TAG, "gap event, %d %s\n", event, esp_gap_ble_cb_event_text[event]);

	switch (event) {
	case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: // was able to set adv data params (after esp_ble_gap_config_adv_data_raw), can start advertize
	case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
		esp_ble_gap_start_advertising(&spp_adv_params);
		break;
	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: // was able to start advertize
											 //advertising start complete event to indicate advertising start successfully or failed
		if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
			ESP_LOGE(TAG, "Advertising start failed\n");
		}
		break;
	case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
		ESP_LOGI(TAG, "update conn params, status %d, min_int %d, max_int %d, latency %d, conn_int %d, timeout %d", param->update_conn_params.status, param->update_conn_params.min_int, param->update_conn_params.max_int, param->update_conn_params.latency, param->update_conn_params.conn_int, param->update_conn_params.timeout);
		break;
	case ESP_GAP_BLE_SEC_REQ_EVT:
		/* send the positive(true) security response to the peer device to accept the security request.
		If not accept the security request, should sent the security response with negative(false) accept value*/
		esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
		ESP_LOGI(GATTS_TABLE_TAG, "SEC_REQ, sending positive response");
		break;
	case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
		ESP_LOGI(GATTS_TABLE_TAG, "PASSKEY_NOTIF, %d", param->ble_security.key_notif.passkey);
		break;
	case ESP_GAP_BLE_KEY_EVT:
		//shows the ble key info share with peer device to the user.
		ESP_LOGI(GATTS_TABLE_TAG, "KEY, key type %d %s", param->ble_security.ble_key.key_type, esp_key_type_to_str(param->ble_security.ble_key.key_type));
		break;
	case ESP_GAP_BLE_AUTH_CMPL_EVT:
		ESP_LOGI(GATTS_TABLE_TAG, "AUTH_CMPL, address type = %d, pair status = %s", param->ble_security.auth_cmpl.addr_type, param->ble_security.auth_cmpl.success ? "success" : "fail");
		//show_bonded_devices();
		break;
	default:
		break;
	}
}

// debug helping list
const char *esp_gatts_cb_event_text[] = {
	"ESP_GATTS_REG_EVT",
	"ESP_GATTS_READ_EVT",
	"ESP_GATTS_WRITE_EVT",
	"ESP_GATTS_EXEC_WRITE_EVT",
	"ESP_GATTS_MTU_EVT",
	"ESP_GATTS_CONF_EVT (confirm notification)",
	"ESP_GATTS_UNREG_EVT",
	"ESP_GATTS_CREATE_EVT",
	"ESP_GATTS_ADD_INCL_SRVC_EVT",
	"ESP_GATTS_ADD_CHAR_EVT",
	"ESP_GATTS_ADD_CHAR_DESCR_EVT",
	"ESP_GATTS_DELETE_EVT",
	"ESP_GATTS_START_EVT",
	"ESP_GATTS_STOP_EVT",
	"ESP_GATTS_CONNECT_EVT",
	"ESP_GATTS_DISCONNECT_EVT",
	"ESP_GATTS_OPEN_EVT",
	"ESP_GATTS_CANCEL_OPEN_EVT",
	"ESP_GATTS_CLOSE_EVT",
	"ESP_GATTS_LISTEN_EVT",
	"ESP_GATTS_CONGEST_EVT",
	"ESP_GATTS_RESPONSE_EVT",
	"ESP_GATTS_CREAT_ATTR_TAB_EVT",
	"ESP_GATTS_SET_ATTR_VAL_EVT"
};

void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
	esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *)param;
	uint8_t res = 0xff;

	ESP_LOGI(TAG, "gatts event, %d %s, gatts if %d\n", event, esp_gatts_cb_event_text[event], gatts_if);
	ESP32BLEModule & module = ESP32BLEModule::getInstance();
	if (module.mGattsIf != ESP_GATT_IF_NONE && module.mGattsIf != gatts_if)
		ESP_LOGW(TAG, "Unexpected gatts_if %d, expecting %d\n", gatts_if, module.mGattsIf);

	switch (event) {
	case ESP_GATTS_REG_EVT:
		if (param->reg.status == ESP_GATT_OK) {
			ESP_LOGI(TAG, "Reg app ok, gatts_if %d, app_id %04x, status %d\n", gatts_if, param->reg.app_id, param->reg.status);
		}
		else {
			ESP_LOGW(TAG, "Reg app failed, app_id %04x, status %d\n", param->reg.app_id, param->reg.status);
			break;
		}

		module.mGattsIf = gatts_if; // save

		ESP_LOGI(TAG, "set_device_name\n" /*, __func__, __LINE__*/);
		esp_ble_gap_set_device_name(DEVICE_NAME);

		//ESP_LOGI(TAG, "config_adv_data_raw\n"/*, __func__, __LINE__*/);
		//esp_ble_gap_config_adv_data_raw((uint8_t *)spp_adv_data_raw, sizeof(spp_adv_data_raw));

		ESP_LOGI(TAG, "config_adv_data\n"/*, __func__, __LINE__*/);
		esp_ble_gap_config_adv_data(&spp_adv_data);

		ESP_LOGI(TAG, "create_attr_tab\n"/*, __func__, __LINE__*/);
		esp_ble_gatts_create_attr_tab(spp_gatt_db, gatts_if, SPP_IDX_NB, SPP_SVC_INST_ID); // here we pass the list of our services and characteristics
		break;
	case ESP_GATTS_READ_EVT: // we have no READ operations in this application
		res = module.findCharAndDesrIndex(p_data->read.handle);
		ESP_LOGI(TAG, "read event for enum %d, conn %d\n", /*__func__, __LINE__,*/ res, p_data->read.conn_id);
		break;
	case ESP_GATTS_WRITE_EVT: { // writes
		res = module.findCharAndDesrIndex(p_data->write.handle);
		ESP_LOGI(TAG, "write event for enum %d, conn %d, is_prep %d\n", /*__func__, __LINE__,*/ res, p_data->write.conn_id, p_data->write.is_prep);
		if (p_data->write.is_prep == false) { // is_prep is a special type of batched write, we don't use it
			if (res == SPP_IDX_SPP_DATA_NTF_CFG) { // the notification enable flag, apparently we need to implement this ourselves
				if ((p_data->write.len == 2) && (p_data->write.value[0] == 0x01) && (p_data->write.value[1] == 0x00)) {
					module.mClients[p_data->write.conn_id].enableNotification = true;
					// update server module
					module.onClientConnectionChange(p_data->write.conn_id, true);
				}
				else if ((p_data->write.len == 2) && (p_data->write.value[0] == 0x00) && (p_data->write.value[1] == 0x00)) {
					module.mClients[p_data->write.conn_id].enableNotification = false;
					// update server module
					module.onClientConnectionChange(p_data->write.conn_id, false); // in this application, notification disabled is like there is no connection
				}
			}
			else if (res == SPP_IDX_SPP_DATA_RECV_VAL) { // the write characteristic
				ESP_LOGI(TAG, "Invoking processIncomingPacket\n");
				module.processIncomingPacket(p_data->write.conn_id, p_data->write.value, p_data->write.len);  // process incoming data
			}
			else {
				// not expected characteristic
			}
		}
		else if ((p_data->write.is_prep == true) && (res == SPP_IDX_SPP_DATA_RECV_VAL)) {
			ESP_LOGW(TAG, "write event, is_prep not implemented\n");
			// is_prep not implemented
		}
		break;
	}
	case ESP_GATTS_EXEC_WRITE_EVT: {
		ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT\n");
		if (p_data->exec_write.exec_write_flag) {
			// some is_prep related logic was here
			ESP_LOGW(TAG, "exec write event, is_prep not implemented\n");
		}
		break;
	}
	case ESP_GATTS_MTU_EVT: // mtu changing event
		module.mClients[p_data->mtu.conn_id].mtu = p_data->mtu.mtu;
		ESP_LOGI(TAG, "New MTU, mtu %d, connection id %d\n", p_data->mtu.mtu, p_data->mtu.conn_id);
		break;
	case ESP_GATTS_CONF_EVT: // receive confirmation event
		break;
	case ESP_GATTS_UNREG_EVT:
		module.mGattsIf = ESP_GATT_IF_NONE;
		ESP_LOGI(TAG, "Unreg app ok, handle %04x, status %d\n", param->create.service_handle, param->create.status);
		break;
	case ESP_GATTS_DELETE_EVT:
		break;
	case ESP_GATTS_START_EVT:
		break;
	case ESP_GATTS_STOP_EVT:
		break;
	case ESP_GATTS_CONNECT_EVT:
	{
		// update connection parameters to have faster timeout
		esp_ble_conn_update_params_t conn_params = { 0 };
		memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
		// For the IOS system, please reference the apple official documents about the ble connection parameters restrictions.
		// https://developer.apple.com/library/content/qa/qa1931/_index.html
		conn_params.latency = 0; // was 0
		conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
		conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
		conn_params.timeout = 400; // timeout = 400*10ms = 4000ms
		ESP_LOGI(GATTS_TABLE_TAG, "sending esp_ble_gap_update_conn_params\n");
		esp_ble_gap_update_conn_params(&conn_params);

		// start security connect with peer device when receive the connect event sent by the master
		esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);

		// handle connection state
		module.mClients[p_data->connect.conn_id].isConnected = true;
		ESP_LOGI(TAG, "connect, connection id %d, address " BT_BD_ADDR_STR "\n", p_data->connect.conn_id, BT_BD_ADDR_HEX(p_data->connect.remote_bda));
		if (++module.mConnectedClientCount < module.MAX_CLIENTS)
			esp_ble_gap_start_advertising(&spp_adv_params); // advertise once again if there is a slot

		// this is not enough to send notify to the fochica server module about a connection, wait for remote side to enable notifications before announcing connection.
	}
	break;
	case ESP_GATTS_DISCONNECT_EVT:
		module.initClient(module.mClients[p_data->disconnect.conn_id], p_data->disconnect.conn_id); // reset state
		module.mConnectedClientCount--;
		ESP_LOGI(TAG, "disonnect, connection id %d\n", p_data->disconnect.conn_id);
		esp_ble_gap_start_advertising(&spp_adv_params); // advertise once again
		// update server module
		module.onClientConnectionChange(p_data->disconnect.conn_id, false);
		break;
	case ESP_GATTS_OPEN_EVT:
		break;
	case ESP_GATTS_CANCEL_OPEN_EVT:
		break;
	case ESP_GATTS_CLOSE_EVT:
		break;
	case ESP_GATTS_LISTEN_EVT:
		break;
	case ESP_GATTS_CONGEST_EVT:
		break;
	case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
		ESP_LOGI(TAG, "The number of handles %d\n", param->add_attr_tab.num_handle);
		if (param->add_attr_tab.status != ESP_GATT_OK) {
			ESP_LOGE(TAG, "Create attribute table failed, error code=0x%x", param->add_attr_tab.status);
		}
		else if (param->add_attr_tab.num_handle != SPP_IDX_NB) {
			ESP_LOGE(TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, SPP_IDX_NB);
		}
		else {
			memcpy(module.mHandleTable, param->add_attr_tab.handles, sizeof(module.mHandleTable));
			esp_ble_gatts_start_service(module.mHandleTable[SPP_IDX_SVC]);
		}
		break;
	}
	default:
		break;
	}
}



#endif // ESP32
