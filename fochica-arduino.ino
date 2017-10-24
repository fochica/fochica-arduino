/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// our new classes, included here automatically on "add code" wizard
// keep only what we need for the main file
#define START_MARKER

/////////////
// MAIN FLAGS
#ifdef  ARDUINO_AVR_MEGA2560
#define SUPPORT_SD_MODULE // The SD library uses over 0.5KB of RAM and lots of Flash memory. The SD module is an SPI devices and takes 4 pins. Practical use is only possible of larger boards, such as the Arduino Mega, not Arduino Uno.
#endif

///////////
// INCLUDES
#include "Nullable.h"
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
#include "PersistentLogImpl_Null.h"
#ifdef SUPPORT_SD_MODULE
#include "PersistentLogImpl_SD.h"
#endif
#include "CalibratedSensorTester.h"
#include "SoundManager.h"
#include "RTCImpl_Sync.h"
#include "RTCImpl_DS1307.h"
#include "GenericBLEModuleClient.h"
#include "SensorCapacitivePressure1Pin.h"
#include "SensorAnalog.h"
#include "SensorSharpIRDistance.h"
#include "SensorDigital.h"
#include "SensorQtouch.h"
#include "SensorVoltage.h"
#include "SensorVcc.h"
#include "SensorFreeRAM.h"
#include "Manager.h"
#include "DebugStream.h"
#include "ConfigVariationBase.h"

// includes of libraries that are used in this project, for the sake of visual micro and IntelliSense
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <EEPROM.h>
#ifdef SUPPORT_SD_MODULE
#include <SD.h>
#endif

///////////
// SETTINGS

// include one ConfigVariation based on your hardware setup and wiring
#include "ConfigVariation-BasicV1.h"
//#include "ConfigVariation-PrototypeMega1.h"
//#include "ConfigVariation-UnoShieldV1.h"
//#include "ConfigVariation-NanoV1.h"

// or make your custom ConfigPrivateVariation-X.h file and use it (ignored by git)
//#include "ConfigPrivateVariation-PrototypeUno1.h"

// make an instance of the config variation
ConfigVariation configVariation; 

// parameters for detecting that engine is running based on battery voltage
const int CAR_ENGINE_ALPHA = 0.2 * CalibratedSensor::MAX_EXP_ALPHA;
const int CAR_ENGINE_OFF_TH = 13000; // 13V
const int CAR_ENGINE_RUNNING_TH = 13500; // 13.5V

//////////
// OBJECTS
// technical sensors
SensorFreeRAM ram("SRAM");
SensorVcc vcc("Vcc");
SensorVoltage bat("Battery", configVariation.getBatteryVoltageSensorAnalogPin(), configVariation.getBatteryVoltageSensorResistorToGroundValue(), configVariation.getBatteryVoltageSensorResistorToVoltageValue());
// TODO, consider to make bat Nullable

// timing and logging
IRTC * rtc = configVariation.getRTC();
IPersistentLog * logger = configVariation.getPersistentLog(rtc);

// discharge protection
DischargeProtectionManager * dischargeProtection = configVariation.getDischargeProtectionManager(&bat);

// car engine running detection
// State A is off and B is running. start with default A state
CalibratedSensor carEngineState(&bat, CAR_ENGINE_ALPHA, CAR_ENGINE_OFF_TH, CAR_ENGINE_RUNNING_TH);

// event handlers
EventHandlerDisconnectedStateChange ehDisconnectedStateChange;
//EventHandlerConnectedStateChange ehConnectedStateChange; // makes a sound on seat state changes, which are aggregated states
EventHandlerConnectedSensorStateChange ehConnectedStateChange; // makes a sound on sensor changes, which are lower level events
EventHandlerFallbackReminder ehFallbackReminder(carEngineState); // doesn't work for cars that turn the engine off automatically during stops
//EventHandlerExternalAlertTrigger ehAlertLed(carEngineState, 10000, LED_BUILTIN); // example of an external alert trigger. turn on-board led (13) as an indication of alert. if using, make sure there is no conflict with other uses of pin 13 (alive LED, SPI for SD card, etc).
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

	// first thing, make sure we get power
	if(dischargeProtection!=NULL)
		dischargeProtection->begin();

	// init buzzer 
	SoundManager::getInstance().setPassiveBuzzer(configVariation.getBuzzerPin(), configVariation.getBuzzerOffState());

	// init serial
	Serial.begin(configVariation.getSerialBaud());
	delay(10); // wait a little for dev env to connect before sending data	

	// debug and log interfaces
	DebugStream = &Serial;
	if (DebugStream)
		DebugStream->println(F("Start"));

	if (DebugStream)
		DebugStream->println(F("Initializing RTC and Persistent logger"));
	rtc->begin();
	manager.setRTC(rtc);
	if (logger->begin()) // if init ok
		PersistentLog = logger;
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
	configVariation.registerSensors(manager.getSensorManager());

	// init comms
	PersistentLogWrite(F("Initializing communications"));
	configVariation.registerClientDevices(manager.getClientManager());
	manager.getClientManager().setReceiverCallback(&manager);

	// init event handlers
	PersistentLogWrite(F("Initializing event handlers"));
	manager.setEventHandlerCount(6);
	manager.addEventHandler(&ehConnectedStateChange);
	manager.addEventHandler(&ehDisconnectedStateChange);
	manager.addEventHandler(&ehFallbackReminder);
	//ehAlertLed.begin();
	//manager.addEventHandler(&ehAlertLed);
	manager.addEventHandler(&ehPersistentLog);
	manager.addEventHandler(&ehClientConnectionChange);

	// alive led
	Nullable<uint8_t> aliveLedPin = configVariation.getAliveLedPin();
	if(aliveLedPin.hasValue())
		pinMode(aliveLedPin, OUTPUT);

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

	// ram dump for debug
	//ram.dumpSRAMContent(Serial);
	//ram.dumpSRAMBounds(Serial);
	//for (;;); // don't proceed to normal operation

	// make start sound
	SoundManager::getInstance().playBeep(BeepType::Start);
}

void loop()
{
	// debug
	if (DebugStream) {
		//DebugStream->println(F("Loop"));
		//DebugStream->println(ram.getValueInt());
		//DebugStream->println(vcc.getValueInt());
		//DebugStream->println(bat.getValueFloat());
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
	// discharge protection
	if(dischargeProtection!=NULL)
		dischargeProtection->work();

	// loop delay and flashing of an "alive" led (if configured as such)
	Nullable<uint8_t> aliveLedPin = configVariation.getAliveLedPin();
	if (aliveLedPin.hasValue())
	{
		long loopDelay = configVariation.getLoopDelay();
		long dutyCycleDelay = loopDelay * configVariation.getAliveLedDutyCyclePercent() / 100;
		digitalWrite(aliveLedPin, HIGH);
		delay(dutyCycleDelay);
		digitalWrite(aliveLedPin, LOW);
		delay(loopDelay - dutyCycleDelay);
	}
	else
	{
		delay(configVariation.getLoopDelay());
	}
}
