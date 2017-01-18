// our classes, added here automatically on "add code" wizard
// keep only what we need for the main file

//#define USE_SD_LIBRARY // The SD library takes over 0.5KB RAM and lots of Flash memory. Practical use is only possible of larger bords, such as the Mega, not Uno.

#include "PacketSensorOperation.h"
#include "SensorOperation.h"
#include "PersistentLog.h"
#include "PersistentLogImpl_Serial.h"
#ifdef USE_SD_LIBRARY
#include "PersistentLogImpl_SD.h"
#endif
#include "CalibratedSensorTester.h"
#include "SoundManager.h"
#include "RTCImpl_Sync.h"
#include "RTCImpl_DS1307.h"
#include "GenericBLEModuleClient.h"
#include "SensorSharpIRDistance.h"
#include "SensorDigital.h"
#include "SensorQtouch.h"
#include "SensorVoltage.h"
#include "SensorVcc.h"
#include "SensorFreeRAM.h"
#include "Manager.h"
#include "DebugStream.h"

// Includes of libraries that are used in this project, for the sake of visual micro and IntelliSense
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <EEPROM.h>
#ifdef USE_SD_LIBRARY
#include <SD.h>
#endif

// settings
#define DEVICE2 // build with sensors and adapter for device #2, used for testing in the lab. Otherwise device #1 used in field testing.

const long SERIAL_BAUD = 115200;

const int BATTERY_VOLTAGE_SENSOR_PIN = 1;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000;
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000;

const int BUZZER_PIN = 4;
#ifdef DEVICE2
const int BUZZER_OFF_STATE = LOW; // LOW is using a NPN transistor (preffered) to drive the buzzer. HIGH if using a PNP.
#else
const int BUZZER_OFF_STATE = HIGH; // LOW is using a NPN transistor (preffered) to drive the buzzer. HIGH if using a PNP.
#endif

const int CAPACITANCE_READ_PIN = 2;
const int CAPACITANCE_REF_PIN = 3;

const int REED_SWITCH_PIN = 6;

const int IR_DISTANCE_READ_PIN = 0;

const int LOOP_DELAY = 1000;

// Bluetooth Low Energy (HM-10 module)
const int BLE_RX_PIN = 8; // yellow
const int BLE_TX_PIN = 9; // orange
const int BLE_STATE_PIN = 7; // gray

// Bluetooth Low Energy (CC41 module)
const int BLE2_RX_PIN = 2; // yellow
const int BLE2_TX_PIN = 3; // orange
const int BLE2_STATE_PIN = 5; // gray

const int SD_CS_PIN = 10;

// objects
// technical sensors
SensorFreeRAM ram("SRAM");
SensorVcc vcc("Vcc");
SensorVoltage bat("Battery", BATTERY_VOLTAGE_SENSOR_PIN, BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND, BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE);
// occupancy (business logic) sensors
SensorQtouch capSense("CapSense", CAPACITANCE_READ_PIN, CAPACITANCE_REF_PIN);
//SensorDigital digitalTest("Test", BLE_STATE_PIN); // just a test, reuse existing pin
SensorDigital digitalReed("Reed", REED_SWITCH_PIN, INPUT_PULLUP);
#ifndef DEVICE2
SensorSharpIRDistance irDistance("IRDistance", IR_DISTANCE_READ_PIN);
// communication devices
SoftwareSerial bleSerial1(BLE_RX_PIN, BLE_TX_PIN);
GenericBLEModuleClient ble1(bleSerial1, BLE_STATE_PIN);
#else
SoftwareSerial bleSerial2(BLE2_RX_PIN, BLE2_TX_PIN);
GenericBLEModuleClient ble2(bleSerial2, BLE2_STATE_PIN);
#endif
// misc
#ifndef DEVICE2
RTCImpl_Sync rtc;
#else
RTCImpl_DS1307 rtc;
#endif
PersistentLogImpl_Serial logger(Serial, rtc); // log to serial
//PersistentLogImpl_SD logger(SD_CS_PIN, rtc); // log to SD card. You will need a Mega or another board with a lot of Flash to fit this support in program memory.
Manager& manager = Manager::getInstance();

void setup()
{
	// init serial
	Serial.begin(SERIAL_BAUD);
	delay(10); // wait a little for dev env to connect before sending data	

	// debug and log interfaces
	DebugStream = &Serial;
	DebugStream->println(F("Start"));
	rtc.begin();
	manager.setRTC(&rtc);
	if (logger.begin()) // if init ok
		PersistentLog = &logger;
	else
		PersistentLog = NULL;

	// testing of low ram conditions
	//char * memGrab = "Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. ";
	//Serial.println(memGrab);

	// testing
	//CalibratedSensorTester test;
	//test.runTests();
	//for (;;); // don't proceed to normal operation

	// init sensors
	manager.getSensorManager().setSeatCount(1);
#ifdef DEVICE2
	manager.getSensorManager().setSensorCount(2);
#else
	manager.getSensorManager().setSensorCount(3);
#endif
	capSense.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::UnderSeat, &capSense);
	digitalReed.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::Chest, &digitalReed);
#ifndef DEVICE2
	irDistance.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::Above, &irDistance);
#endif
	//digitalTest.begin();
	//manager.getSensorManager().addSensor(0, SensorLocation::Chest, &digitalTest);

	// init tech sensors and params
	vcc.begin();
	manager.getTechnicalManager().setVccSensor(&vcc);
	bat.begin();
	manager.getTechnicalManager().setCarBatteryVoltageSensor(&bat);
	ram.begin();
	manager.getTechnicalManager().setFreeRAMSensor(&ram);
	
	// init comms
	manager.getClientManager().setDeviceCount(1);
#ifndef DEVICE2
	ble1.begin();
	manager.getClientManager().addDevice(&ble1);
#else
	ble2.begin();
	manager.getClientManager().addDevice(&ble2);
#endif
	manager.getClientManager().setReceiverCallback(&manager);

	// misc
	DebugStream->println(F("Free RAM: "));
	DebugStream->println(ram.getValueInt());

	// ram dump
	//ram.dumpSRAMContent(Serial);
	//ram.dumpSRAMBounds(Serial);
	//for (;;); // don't proceed to normal operation

	// init buzzer and make start sound
	// TODO, change to a fun tune
	SoundManager::getInstance().setPassiveBuzzer(BUZZER_PIN, BUZZER_OFF_STATE);
	SoundManager::getInstance().playBeep(BeepType::Start);
}

void loop()
{
	// debug
	if (DebugStream) {
		//DebugStream->println(F("Loop"));
		DebugStream->println(ram.getValueInt());
		//DebugStream->println(vcc.getValueFloat());
		//DebugStream->println(bat.getValueFloat());
#ifndef DEVICE2
		//DebugStream->println(ble1.isConnected());
		//DebugStream->println(irDistance.getValueInt());
#endif
		//DebugStream->println(capSense.getValueInt());
		//DebugStream->println(digitalReed.getValueInt());
	}

	if (PersistentLog) {
		Print & f = PersistentLog->open();
		f.println(F("Test log"));
		PersistentLog->close(f);
	}

	// work
	manager.work();

	delay(LOOP_DELAY);
}
