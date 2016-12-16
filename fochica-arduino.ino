// our classes, added here automatically on "add code" wizard
// keep only what we need for the main file

#include "CalibratedSensorTester.h"
#include "SoundManager.h"
#include "RTCImpl_DS1307.h"
#include "GenericBLEModuleClient.h"
#include "SensorDigital.h"
#include "SensorQtouch.h"
#include "SensorVoltage.h"
#include "SensorVcc.h"
#include "SensorFreeRAM.h"
#include "Manager.h"
#include "DebugStream.h"

// Includes of libraries for the sake of visual micro and IntelliSense
#include <SoftwareSerial.h>
#include <RTClib.h>

// settings
const long SERIAL_BAUD = 115200;

const int BATTERY_VOLTAGE_SENSOR_PIN = 1;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000;

const int BUZZER_PIN = 4;

const int CAPACITANCE_READ_PIN = 2;
const int CAPACITANCE_REF_PIN = 3;

const int REED_SWITCH_PIN = 6;

const int LOOP_DELAY = 1000;

// Bluetooth Low Energy (HM-10 module)
const int BLE_RX_PIN = 8; // yellow
const int BLE_TX_PIN = 9; // orange
const int BLE_STATE_PIN = 7; // gray

// Bluetooth Low Energy (CC41 module)
const int BLE2_RX_PIN = 2; // yellow
const int BLE2_TX_PIN = 3; // orange
const int BLE2_STATE_PIN = 5; // gray

// objects
// technical sensors
SensorFreeRAM ram("SRAM");
SensorVcc vcc("Vcc");
SensorVoltage bat("Battery", BATTERY_VOLTAGE_SENSOR_PIN, BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND, BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE);
// occupancy (business logic) sensors
SensorQtouch capSense("CapSense", CAPACITANCE_READ_PIN, CAPACITANCE_REF_PIN);
//SensorDigital digital("Test", BLE_STATE_PIN); // just a test, reuse existing pin
SensorDigital digital("Reed", REED_SWITCH_PIN, INPUT_PULLUP); // just a test, reuse existing pin
// communication devices
SoftwareSerial bleSerial1(BLE_RX_PIN, BLE_TX_PIN);
GenericBLEModuleClient ble1(bleSerial1, BLE_STATE_PIN);
SoftwareSerial bleSerial2(BLE2_RX_PIN, BLE2_TX_PIN);
GenericBLEModuleClient ble2(bleSerial2, BLE2_STATE_PIN);
// misc
RTCImpl_DS1307 rtc;
Manager& manager = Manager::getInstance();

void setup()
{
	// init serial
	Serial.begin(SERIAL_BAUD);
	delay(10); // wait a little for dev env to connect before sending data
	Serial.println("test");
	delay(2000);
	DebugStream = &Serial;
	DebugStream->println(F("Start"));

	// testing
	//CalibratedSensorTester test;
	//test.runTests();
	//for (;;); // don't proceed to normal operation

	// init sensors
	manager.getSensorManager().setSeatCount(2);
	manager.getSensorManager().setSensorCount(2);
	capSense.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::UnderSeat, &capSense);
	digital.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::Chest, &digital);
	
	// init tech sensors and params
	vcc.begin();
	manager.getTechnicalManager().setVccSensor(&vcc);
	bat.begin();
	manager.getTechnicalManager().setCarBatteryVoltageSensor(&bat);
	ram.begin();
	manager.getTechnicalManager().setFreeRAMSensor(&ram);
	
	// init comms
	manager.getClientManager().setDeviceCount(2);
	ble1.begin();
	manager.getClientManager().addDevice(&ble1);
	ble2.begin();
	manager.getClientManager().addDevice(&ble2);
	manager.getClientManager().setReceiverCallback(&manager);

	// misc
	DebugStream->println(F("Free RAM: "));
	DebugStream->println(ram.getValueInt());
	rtc.begin();
	manager.setRTC(&rtc);

	// init buzzer and make start sound
	// TODO, change to a fun tune
	SoundManager::getInstance().setPassiveBuzzer(BUZZER_PIN);
	SoundManager::getInstance().playBeep(BeepType::Start);
}

void loop()
{
	// debug
	DebugStream->println(F("Loop"));
	DebugStream->println(ram.getValueInt());
	DebugStream->println(vcc.getValueFloat());
	DebugStream->println(bat.getValueFloat());
	DebugStream->println(capSense.getValueInt());
	DebugStream->println(ble1.isConnected());
	DebugStream->println(digital.getValueInt());

	// work
	manager.work();

	delay(LOOP_DELAY);
}
