// our classes
#include "SensorDigital.h"
#include "PacketSensorData.h"
#include "PacketTechnicalData.h"
#include "PacketHeader.h"
#include "GenericBLEModuleClient.h"
#include "SensorQtouch.h"
#include "SoundManager.h"
#include "SensorVoltage.h"
#include "SensorVcc.h"
#include "SensorFreeRAM.h"
#include "ISensor.h"
#include "Manager.h"
#include "DebugStream.h"

// Includes of libraries for the sake of visual micro and IntelliSense
#include <SoftwareSerial.h>

// settings
const int SERIAL_BAUD = 9600;

const int BATTERY_VOLTAGE_SENSOR_PIN = 1;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000;

const int BUZZER_PIN = 4;

const int CAPACITANCE_READ_PIN = 2;
const int CAPACITANCE_REF_PIN = 3;

const int LOOP_DELAY = 1000;

// Bluetooth Low Energy (HC-05 module)
const int BLE_RX_PIN = 8; // yellow
const int BLE_TX_PIN = 9; // orange
const int BLE_SENSE_PIN = 5; // gray

// objects
SensorFreeRAM ram("SRAM");
SensorVcc vcc("Vcc");
SensorVoltage bat("Battery", BATTERY_VOLTAGE_SENSOR_PIN, BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND, BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE);
SensorQtouch capSense("CapSense", CAPACITANCE_READ_PIN, CAPACITANCE_REF_PIN);
SensorDigital digital("Test", BLE_SENSE_PIN); // just a test, reuse existing pin
GenericBLEModuleClient ble(BLE_RX_PIN, BLE_TX_PIN, BLE_SENSE_PIN);

void setup()
{
	Serial.begin(SERIAL_BAUD);
	ble.begin();
	delay(10);

	DebugStream = &Serial;
	DebugStream->println("Start");

	DebugStream->println(ram.getValueInt());

	SoundManager::getInstance().setPassiveBuzzer(BUZZER_PIN);
	SoundManager::getInstance().playBeep(BeepType::Error);
}

void loop()
{
	// debug
	DebugStream->println(vcc.getValueFloat());
	DebugStream->println(bat.getValueFloat());
	DebugStream->println(capSense.getValueInt());
	DebugStream->println(ram.getValueInt());
	DebugStream->println(ble.isConnected());
	DebugStream->println(digital.getValueInt());

	// send technical packet
	PacketTechnicalData packet;
	packet.carBatteryCurrent = 0;
	packet.carBatteryVoltage = bat.getValueFloat();
	packet.freeRAM = ram.getValueInt();
	packet.vcc = vcc.getValueFloat();
	ble.sendTechnicalData(packet);

	delay(LOOP_DELAY);
}
