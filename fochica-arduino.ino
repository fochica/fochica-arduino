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
#include "SoundManager.h"
#include "Manager.h"
#include "RNGUtils.h"
#include "DebugStream.h"
#include "SensorMock.h"

// includes of libraries that are used in this project, for the sake of visual micro and IntelliSense
#ifdef __AVR__ // default SoftwareSerial is AVR specific, some other platforms have compatible implementations which can be included here for those platforms
#include <SoftwareSerial.h>
#endif
#include <RTClib.h>
#include <EEPROM.h>
#ifdef SUPPORT_SD_MODULE
#include <SD.h>
#endif

///////////
// SETTINGS

// include one ConfigVariation based on your hardware setup and wiring
#ifdef __AVR__
#include "ConfigVariation-BasicV1.h"
//#include "ConfigVariation-PrototypeMega1.h"
//#include "ConfigVariation-UnoShieldV1.h"
//#include "ConfigVariation-NanoV1.h"
#endif
#ifdef ESP32
#include "ConfigVariation-ESP32V1.h"
#endif

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
// start mode detector
StartModeDetector * startModeDetector = configVariation.getStartModeDetector();

// technical sensors
ISensor * ram = configVariation.getFreeRAMSensor();
ISensor * vcc = configVariation.getVccSensor();
ISensor * bat = configVariation.getBatterySensor();

// timing and logging
IRTC * rtc = configVariation.getRTC();
IPersistentLog * logger = configVariation.getPersistentLog(rtc);

// discharge protection
DischargeProtectionManager * dischargeProtection = NULL;

// car engine running detection, depends on bat voltage sensing
// State A is off and B is running. start with default A state
CalibratedSensor carEngineState(bat != NULL ? bat : (new SensorMock(SensorMockType::Fixed, 0, 0, 0)), // TODO, can we make this optional if no feature or no sensing?
	CAR_ENGINE_ALPHA, CAR_ENGINE_OFF_TH, CAR_ENGINE_RUNNING_TH);

// general manager
Manager& manager = Manager::getInstance();

///////
// CODE
void setup()
{
	// init detector ASAP
	if (startModeDetector != NULL)
		startModeDetector->begin();

	// init tech sensors and params
	if (vcc != NULL) {
		vcc->begin();
		manager.getTechnicalManager().setVccSensor(vcc);
	}
	if (bat != NULL) {
		bat->begin();
		manager.getTechnicalManager().setCarBatteryVoltageSensor(bat);
	}
	if (ram != NULL) {
		ram->begin();
		manager.getTechnicalManager().setFreeRAMSensor(ram);
	}

	// first thing, make sure we get power
	if(bat!=NULL)
		dischargeProtection=configVariation.getDischargeProtectionManager(bat);
	if(dischargeProtection!=NULL) // if created
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
		DebugStream->println(F("Init RTC and Persistent logger"));
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
	PersistentLogWrite(F("Init Sensors"));
	configVariation.registerSensors(manager.getSensorManager());

	// init comms
	PersistentLogWrite(F("Init comms"));
	configVariation.registerClientDevices(manager.getClientManager());
	manager.getClientManager().setReceiverCallback(&manager);

	// init event handlers
	PersistentLogWrite(F("Init event handlers"));
	manager.setEventHandlerCount(configVariation.getEventHandlerCount());
	configVariation.registerEventHandlers(manager.getEventHandlerManager(), carEngineState);

	// alive led
	Nullable<uint8_t> aliveLedPin = configVariation.getAliveLedPin();
	if(aliveLedPin.hasValue())
		pinMode(aliveLedPin, OUTPUT);

	// misc log
	if (ram != NULL) {
		if (DebugStream) {
			DebugStream->print(F("Free RAM: "));
			DebugStream->println(ram->getValueInt());
		}
		Print * persistentFile;
		if (PersistentLog)
			persistentFile = PersistentLog->open();
		if (persistentFile) {
			persistentFile->print(F("Free RAM: "));
			persistentFile->println(ram->getValueInt());
			PersistentLog->close(); // close persistent log of the start process
		}

		// ram dump for debug
		//ram->dumpSRAMContent(Serial);
		//ram->dumpSRAMBounds(Serial);
		//for (;;); // don't proceed to normal operation
	}

	// seed RNG, for some devices (such as ESP32), this needs to happen after communication device has been initialized
	RNGUtils::seed();

	// make start sound
	BeepType::e startBeep = BeepType::Start;
	if (startModeDetector != NULL) {
		if (startModeDetector->getMode() == StartMode::PowerCycle)
			startBeep = BeepType::StartPowerCycle;
		else if (startModeDetector->getMode() == StartMode::Restart)
			startBeep = BeepType::StartRestart;
	}
	SoundManager::getInstance().playBeep(startBeep);

	// debug start mode
	if (startModeDetector != NULL) {
		//delay(5000); // allow for some time to reconnect serial
		startModeDetector->debug();
	}
}

void loop()
{
	configVariation.loop();

	// debug
	/*if (DebugStream) {
		DebugStream->println(F("Loop"));
		if(ram!=NULL)
			DebugStream->println(ram->getValueInt());
		if(vcc!=NULL)
			DebugStream->println(vcc->getValueInt());
		if(bat!=NULL)
			DebugStream->println(bat->getValueFloat());
	}*/

	// Fit program in limited flash of smaller AVRs
#ifndef __AVR_ATmega328P__
	if (PersistentLog) {
		Print * f = PersistentLog->open();
		if (f) {
			f->print(F("Loop, RAM: "));
			f->print(ram->getValueInt());
			f->print(F(", Vcc: "));
			f->print(vcc->getValueFloat());
			f->print(F(", Vbat: "));
			f->println(bat->getValueFloat());
			PersistentLog->close();
		}
	}
#endif

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
