// our new classes, included here automatically on "add code" wizard
// keep only what we need for the main file
#define MARKER

/////////////
// MAIN FLAGS
//#define DEVICE_LAB1 // build with sensors and adapter for device "Lab 1", used for testing in the lab. Uno with one seat.
#define DEVICE_CAR1 // build with sensors and adapter for device "Beta 1", used for real senario testing. Mega with two seats and SD log.

#ifdef  ARDUINO_AVR_MEGA2560
#define SUPPORT_SD_MODULE // The SD library uses over 0.5KB of RAM and lots of Flash memory. The SD module is an SPI devices and takes 4 pins. Practical use is only possible of larger boards, such as the Arduino Mega, not Arduino Uno.
#endif
#ifdef SUPPORT_SD_MODULE
#define USE_SD_MODULE // by default, use it if we can support it
#endif

//#define USE_DISCHARGE_PROTECTION

///////////
// INCLUDES
#include "EventHandlerNotifyClientConnectionChange.h"
#include "EventHandlerWriteToPersistentLog.h"
#include "EventHandlerExternalAlertTrigger.h"
#include "EventHandlerFallbackReminder.h"
#include "EventHandlerConnectedSensorStateChange.h"
#include "EventHandlerDisconnectedStateChange.h"
#include "EventHandlerConnectedStateChange.h"
#include "DischargeProtectionManager.h"
#include "PersistentLog.h"
#include "PersistentLogImpl_Serial.h"
#ifdef SUPPORT_SD_MODULE
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
#ifdef SUPPORT_SD_MODULE
#include <SD.h>
#endif

///////////
// SETTINGS
const long SERIAL_BAUD = 115200;

const int BATTERY_VOLTAGE_SENSOR_PIN = 1; // analog pin#
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000; // 10Kohm
const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000; // 20Kohm

const int BUZZER_PIN = 4;
#ifdef DEVICE_LAB1
const int BUZZER_OFF_STATE = LOW; // LOW is using a NPN transistor (preffered) to drive the buzzer. HIGH if using a PNP.
#else defined(DEVICE_CAR1)
const int BUZZER_OFF_STATE = HIGH; // LOW is using a NPN transistor (preffered) to drive the buzzer. HIGH if using a PNP.
#endif

// capacitance sensor for seat A
const int CAPACITANCE_READ_PIN = 2; // analog pin#
const int CAPACITANCE_AUX_PIN = 3; // analog pin#

const int REED_SWITCH_PIN = 6;

// capacitance sensor for seat B. Uses the aux pin of capacitance sensor of seat A and vise-versa.
const int CAPACITANCE_B_READ_PIN = 3; // analog pin#
const int CAPACITANCE_B_AUX_PIN = 2; // analog pin#

const int REED_SWITCH_B_PIN = A0;

//const int IR_DISTANCE_READ_PIN = 0; // analog pin#

const int LOOP_DELAY = 1000; // seconds

// Bluetooth Low Energy (HM-10 module)
const int BLE_RX_PIN = 8; // yellow
const int BLE_TX_PIN = 9; // orange
const int BLE_STATE_PIN = 7; // gray

// Bluetooth Low Energy (CC41 module)
const int BLE2_RX_PIN = 2; // yellow
const int BLE2_TX_PIN = 3; // orange
const int BLE2_STATE_PIN = 5; // gray

#ifdef USE_SD_MODULE
const int SD_CS_PIN = 10; // depends on module, probably pin 10. need to free it from being used by other component and allocate here.
// if you shield doesn't support Arduino Mega SPI pins, you might need to override the defaults here
const int SD_MOSI_PIN = 11; // 51 native on Mega
const int SD_MISO_PIN = 12; // 50 native on Mega
const int SD_SCK_PIN = 13; // 52 native on Mega
//const int SD_MOSI_PIN = -1; // use default
//const int SD_MISO_PIN = -1; // use default
//const int SD_SCK_PIN = -1; // use default
#endif

#ifdef USE_DISCHARGE_PROTECTION
// Discharge protection "keep-alive" pin
#ifndef USE_SD_MODULE // if we are not using pin 10 for SD card
const int DISCHARGE_PROTECTION_PIN = 10;
#else
#error Allocate and define DISCHARGE_PROTECTION_PIN
#endif
const int DISCHARGE_PROTECTION_ALPHA = 0.02 * CalibratedSensor::MAX_EXP_ALPHA; // can try values closer to 0.01 if discharge protection kicks in during engine cranking.
const int DISCHARGE_PROTECTION_LOW_CHARGE_TH = 11500; // 11.5V
const int DISCHARGE_PROTECTION_HIGH_CHARGE_TH = 12000; // 12V
#endif

const int CAR_ENGINE_ALPHA = 0.2 * CalibratedSensor::MAX_EXP_ALPHA;
const int CAR_ENGINE_OFF_TH = 13000; // 13V
const int CAR_ENGINE_RUNNING_TH = 13500; // 13.5V

//////////
// OBJECTS
// technical sensors
SensorFreeRAM ram("SRAM");
SensorVcc vcc("Vcc");
SensorVoltage bat("Battery", BATTERY_VOLTAGE_SENSOR_PIN, BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND, BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE);

// occupancy (business logic) sensors
SensorQtouch capSense("CapSense", CAPACITANCE_READ_PIN, CAPACITANCE_AUX_PIN);
//SensorDigital digitalTest("Test", BLE_STATE_PIN); // just a test, reuse existing pin
SensorDigital digitalReed("Reed", REED_SWITCH_PIN, INPUT_PULLUP);
//SensorSharpIRDistance irDistance("IRDistance", IR_DISTANCE_READ_PIN);
#ifdef DEVICE_CAR1
SensorQtouch capSenseB("CapSenseB", CAPACITANCE_B_READ_PIN, CAPACITANCE_B_AUX_PIN);
SensorDigital digitalReedB("ReedB", REED_SWITCH_B_PIN, INPUT_PULLUP);
#endif // DEVICE_CAR1

// communication devices
#ifdef DEVICE_LAB1
#ifdef HAVE_HWSERIAL1
GenericBLEModuleClient ble2(Serial1, BLE2_STATE_PIN);
#else
SoftwareSerial bleSerial2(BLE2_RX_PIN, BLE2_TX_PIN);
GenericBLEModuleClient ble2(bleSerial2, BLE2_STATE_PIN);
#endif
#else
#ifdef HAVE_HWSERIAL1
GenericBLEModuleClient ble1(Serial1, BLE_STATE_PIN);
#else
SoftwareSerial bleSerial1(BLE_RX_PIN, BLE_TX_PIN);
GenericBLEModuleClient ble1(bleSerial1, BLE_STATE_PIN);
#endif
#endif

// timing and logging
#if defined(DEVICE_LAB1) || defined(DEVICE_CAR1)
RTCImpl_DS1307 rtc;
#else
RTCImpl_Sync rtc; // when hardware RTC is not available
#endif
#ifdef USE_SD_MODULE
PersistentLogImpl_SD logger(rtc, SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN); // log "persistent data" to SD card. You will need an Arduino Mega or another board with a lot of Flash to fit this support in program memory.
//PersistentLogImpl_Serial logger(Serial, rtc); // log "persistent data" to serial
#else
PersistentLogImpl_Serial logger(Serial, rtc); // log "persistent data" to serial
#endif

// discharge protection
#ifdef USE_DISCHARGE_PROTECTION
CalibratedSensor lowBatCharge(&bat, DISCHARGE_PROTECTION_ALPHA, DISCHARGE_PROTECTION_LOW_CHARGE_TH, DISCHARGE_PROTECTION_HIGH_CHARGE_TH, CalibratedSensorState::B, DISCHARGE_PROTECTION_HIGH_CHARGE_TH); // set to start in state=B="high charge"
DischargeProtectionManager dischargeProtection(lowBatCharge, DISCHARGE_PROTECTION_PIN);
#endif

// car engine running detection
// State A is off and B is running. start with default A state
CalibratedSensor carEngineState(&bat, CAR_ENGINE_ALPHA, CAR_ENGINE_OFF_TH, CAR_ENGINE_RUNNING_TH);

// event handlers
EventHandlerDisconnectedStateChange ehDisconnectedStateChange;
//EventHandlerConnectedStateChange ehConnectedStateChange; // makes a sound on seat state changes, which are aggregated states
EventHandlerConnectedSensorStateChange ehConnectedStateChange; // makes a sound on sensor changes, which are lower level events
EventHandlerFallbackReminder ehFallbackReminder(carEngineState); // doesn't work for cars that turn the engine off automatically during stops
EventHandlerExternalAlertTrigger ehAlertLed(carEngineState, 10000, 13); // example of an external alert trigger. turn on-board led (13) as an indication of alert.
EventHandlerWriteToPersistentLog ehPersistentLog; // writes events to persistent log
EventHandlerNotifyClientConnectionChange ehClientConnectionChange; // makes a sound notification when an adapter to a client device connects or disconnects.

// general manager
Manager& manager = Manager::getInstance();

///////
// CODE
void setup()
{
	// init tech sensors and params
	vcc.begin();
	manager.getTechnicalManager().setVccSensor(&vcc);
	bat.begin();
	manager.getTechnicalManager().setCarBatteryVoltageSensor(&bat);
	ram.begin();
	manager.getTechnicalManager().setFreeRAMSensor(&ram);

#ifdef USE_DISCHARGE_PROTECTION
	// first thing, make sure we get power
	dischargeProtection.begin();
#endif

	// init buzzer 
	SoundManager::getInstance().setPassiveBuzzer(BUZZER_PIN, BUZZER_OFF_STATE);

	// init serial
	Serial.begin(SERIAL_BAUD);
	delay(10); // wait a little for dev env to connect before sending data	

	// debug and log interfaces
	DebugStream = &Serial;
	if (DebugStream)
		DebugStream->println(F("Start"));

	if (DebugStream)
		DebugStream->println(F("Initializing RTC and Persistent logger"));
	rtc.begin();
	manager.setRTC(&rtc);
	if (logger.begin()) // if init ok
		PersistentLog = &logger;
	else {
		PersistentLog = NULL;
		SoundManager::getInstance().playBeep(BeepType::Error);
	}

	//if (DebugStream)
	//	DebugStream->println(F("Making first log entry."));
	PersistentLogWrite(F("Start"));

	// testing of low ram conditions
	//char * memGrab = "Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. Memory grab. ";
	//Serial.println(memGrab);

	// testing
	//CalibratedSensorTester test;
	//test.runTests();
	//for (;;); // don't proceed to normal operation

	// init sensors
	PersistentLogWrite(F("Initializing Sensors"));
#ifdef DEVICE_LAB1
	manager.getSensorManager().setSeatCount(1);
	manager.getSensorManager().setSensorCount(2);
#elif defined(DEVICE_CAR1)
	manager.getSensorManager().setSeatCount(2);
	manager.getSensorManager().setSensorCount(4);
#else
#error No device was defined
#endif
	capSense.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::UnderSeat, &capSense);
	digitalReed.begin();
	manager.getSensorManager().addSensor(0, SensorLocation::Chest, &digitalReed);
#ifdef DEVICE_CAR1
	capSenseB.begin();
	manager.getSensorManager().addSensor(1, SensorLocation::UnderSeat, &capSenseB);
	digitalReedB.begin();
	manager.getSensorManager().addSensor(1, SensorLocation::Chest, &digitalReedB);
#endif
#ifndef DEVICE_LAB1
	//irDistance.begin();
	//manager.getSensorManager().addSensor(0, SensorLocation::Above, &irDistance);
#endif
	//digitalTest.begin();
	//manager.getSensorManager().addSensor(0, SensorLocation::Chest, &digitalTest);

	// init comms
	PersistentLogWrite(F("Initializing communications"));
	manager.getClientManager().setDeviceCount(1);
#ifdef DEVICE_LAB1
	ble2.begin();
	manager.getClientManager().addDevice(&ble2);
#else
	ble1.begin();
	manager.getClientManager().addDevice(&ble1);
#endif
	manager.getClientManager().setReceiverCallback(&manager);

	// init event handlers
	PersistentLogWrite(F("Initializing event handlers"));
	manager.setEventHandlerCount(6);
	manager.addEventHandler(&ehConnectedStateChange);
	manager.addEventHandler(&ehDisconnectedStateChange);
	manager.addEventHandler(&ehFallbackReminder);
	ehAlertLed.begin();
	manager.addEventHandler(&ehAlertLed);
	manager.addEventHandler(&ehPersistentLog);
	manager.addEventHandler(&ehClientConnectionChange);

	// misc log
	if (DebugStream) {
		DebugStream->print(F("Free RAM: "));
		DebugStream->println(ram.getValueInt());
	}
	Print * persistentFile;
	if (PersistentLog)
		persistentFile = PersistentLog->open();
	if (persistentFile) {
		persistentFile->print(F("Free RAM: "));
		persistentFile->println(ram.getValueInt());
		PersistentLog->close(); // close persistent log of the start process
	}

	// ram dump
	//ram.dumpSRAMContent(Serial);
	//ram.dumpSRAMBounds(Serial);
	//for (;;); // don't proceed to normal operation

	// make start sound
	// TODO, change to a fun tune
	SoundManager::getInstance().playBeep(BeepType::Start);
}

void loop()
{
	// debug
	if (DebugStream) {
		//DebugStream->println(F("Loop"));
		//DebugStream->println(ram.getValueInt());
		//DebugStream->println(vcc.getValueFloat());
		//DebugStream->println(bat.getValueFloat());
#ifndef DEVICE_LAB1
		//DebugStream->println(ble1.isConnected());
		//DebugStream->println(irDistance.getValueInt());
#endif
		//DebugStream->println(capSense.getValueInt());
		//DebugStream->println(digitalReed.getValueInt());
	}

	if (PersistentLog) {
		Print * f = PersistentLog->open();
		if (f) {
			f->print(F("Loop, RAM: "));
			f->print(ram.getValueInt());
			f->print(F(", Vcc: "));
			f->print(vcc.getValueFloat());
			f->print(F(", Vbat: "));
			f->print(bat.getValueFloat());
			f->println();
			PersistentLog->close();
		}
	}

	// work
	manager.work();
#ifdef USE_DISCHARGE_PROTECTION
	dischargeProtection.work();
#endif

	delay(LOOP_DELAY);
}
