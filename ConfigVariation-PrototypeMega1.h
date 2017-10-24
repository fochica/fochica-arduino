/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIGVARIATION_h
#define _CONFIGVARIATION_h

#ifndef ARDUINO_AVR_MEGA2560
#error This configuration is assuming Mega build settings
#endif

// Configuration variations for the Mega prototype
// 1 BLEm 4 sensors and 2 seats
// Uses a SD card for persistent log
class ConfigVariation : public ConfigVariationBase
{
public:
	ConfigVariation();

	virtual IRTC * getRTC() { return new RTCImpl_DS1307; }
	virtual IPersistentLog * getPersistentLog(IRTC * rtc) {
		//return new PersistentLogImpl_Serial(Serial, *rtc); // log "persistent data" to serial
		//return new PersistentLogImpl_Null(); // log "persistent data" to null, to reduce verbosity
		return new PersistentLogImpl_SD(*rtc, SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN); // log "persistent data" to SD card.
	}

	virtual uint8_t getBuzzerOffState() { return HIGH; } // this prototype uses PNP transistor for the buzzer
	virtual void registerSensors(SensorManager & sm);

protected:

private:
	// capacitive touch sensor
	const int CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN = 2; // analog pin#
	const int CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN = 3; // analog pin#

	// chest buckle sensor
	const int REED_SWITCH_SENSOR_PIN = 6;

	// capacitive touch sensor
	const int CAPACITIVE_TOUCH_SENSOR2_READ_ANALOG_PIN = 3; // analog pin#
	const int CAPACITIVE_TOUCH_SENSOR2_AUX_ANALOG_PIN = 2; // analog pin#

	// chest buckle sensor
	const int REED_SWITCH_SENSOR2_PIN = A0;

	/// pressure sensors
	const int FSR_SENSOR_ANALOG_PIN = 8; // analog pin#
	const int CAPACITIVE_PRESSURE_SENSOR_PIN = A10;
	const int FSR_SENSOR2_ANALOG_PIN = 9; // analog pin#
	const int CAPACITIVE_PRESSURE_SENSOR2_PIN = A11;

	// Uno SD shield, an old type that doesn't support Mega naturally
	const int SD_CS_PIN = 10; // depends on SD shield module, probably pin 10. need to free it from being used by other component and allocate here.
	// if you shield doesn't support Arduino Mega hardware SPI pins, you might need to override the defaults here
	const int SD_MOSI_PIN = 11; // 51 native on Mega
	const int SD_MISO_PIN = 12; // 50 native on Mega
	const int SD_SCK_PIN = 13; // 52 native on Mega
	//const int SD_MOSI_PIN = -1; // use default
	//const int SD_MISO_PIN = -1; // use default
	//const int SD_SCK_PIN = -1; // use default
};

ConfigVariation::ConfigVariation()
{

}

void ConfigVariation::registerSensors(SensorManager & sm)
{
	SensorQtouch * capSense = new SensorQtouch("CapSense", CAPACITIVE_TOUCH_SENSOR_READ_ANALOG_PIN, CAPACITIVE_TOUCH_SENSOR_AUX_ANALOG_PIN);
	SensorDigital * digitalReed = new SensorDigital("Reed", REED_SWITCH_SENSOR_PIN, INPUT_PULLUP);
	SensorAnalog * fsr = new SensorAnalog("FSR", FSR_SENSOR_ANALOG_PIN, SensorType::FSR);
	SensorCapacitivePressure1Pin * capPressure = new SensorCapacitivePressure1Pin("CapPressure", CAPACITIVE_PRESSURE_SENSOR_PIN);
	SensorQtouch * capSense2 = new SensorQtouch("CapSense2", CAPACITIVE_TOUCH_SENSOR2_READ_ANALOG_PIN, CAPACITIVE_TOUCH_SENSOR2_AUX_ANALOG_PIN);
	SensorDigital * digitalReed2 = new SensorDigital("Reed2", REED_SWITCH_SENSOR2_PIN, INPUT_PULLUP);
	SensorAnalog * fsr2 = new SensorAnalog("FSR2", FSR_SENSOR2_ANALOG_PIN, SensorType::FSR);
	SensorCapacitivePressure1Pin * capPressure2 = new SensorCapacitivePressure1Pin("CapPressure2", CAPACITIVE_PRESSURE_SENSOR2_PIN);

	sm.setSeatCount(2);
	sm.setSensorCount(8);

	capSense->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capSense);
	digitalReed->begin();
	sm.addSensor(0, SensorLocation::Chest, digitalReed);
	fsr->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, fsr);
	capPressure->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capPressure);

	capSense2->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, capSense2);
	digitalReed2->begin();
	sm.addSensor(1, SensorLocation::Chest, digitalReed2);
	fsr->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, fsr2);
	capPressure->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, capPressure2);
}

#else
#error Only one ConfigVariation can be included
#endif