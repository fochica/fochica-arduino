/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ConfigVariationBase.h

#ifndef _CONFIGVARIATIONBASE_h
#define _CONFIGVARIATIONBASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ClientManager.h"
#include "SensorManager.h"
#include "RTCImpl_Sync.h"
#include "Nullable.h"
#include "PersistentLogImpl_Null.h"
#include "PersistentLogImpl_Serial.h"
#include "DischargeProtectionManager.h"

// A base abstract class for configuration variations. Also holds some basic default configuration.
// Don't hold memory wasting objects that might not be relevant for the derived classes.
class ConfigVariationBase
{
public:
	ConfigVariationBase();

	virtual long getSerialBaud() { return SERIAL_BAUD; }

	virtual long getLoopDelay() { return LOOP_DELAY; }

	virtual uint8_t getBatteryVoltageSensorAnalogPin() { return BATTERY_VOLTAGE_SENSOR_ANALOG_PIN; }
	virtual long getBatteryVoltageSensorResistorToGroundValue() { return BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND; }
	virtual long getBatteryVoltageSensorResistorToVoltageValue() { return BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE; }

	virtual uint8_t getBuzzerPin() { return BUZZER_PIN; }
	virtual uint8_t getBuzzerOffState() { return BUZZER_OFF_STATE; }

	virtual IRTC * getRTC() { return new RTCImpl_Sync; } // when hardware RTC is not available
	virtual IPersistentLog * getPersistentLog(IRTC * rtc) {
		//return new PersistentLogImpl_Serial(Serial, *rtc); // log "persistent data" to serial
		return new PersistentLogImpl_Null(); // log "persistent data" to null, to reduce verbosity
	}
	virtual DischargeProtectionManager * getDischargeProtectionManager(ISensor * batterySensor) { return NULL; }

	virtual Nullable<uint8_t> getAliveLedPin() { return Nullable<uint8_t>(); } // "alive" led, flashes during operation. off by defaults, due to common conficts with SPI and lack of LED_BUILTIN in all platforms
	virtual int getAliveLedDutyCyclePercent() { return ALIVE_LED_DUTY_CYCLE_PERCENT; }

	virtual ISensor * getVccSensor() = 0;
	virtual ISensor * getFreeRAMSensor() = 0;
	virtual ISensor * getBatterySensor() = 0;

	virtual void registerClientDevices(ClientManager & cm) = 0;
	virtual void registerSensors(SensorManager & sm) = 0;

protected:
	// consts
	const long SERIAL_BAUD = 115200;

	const int LOOP_DELAY = 1000; // seconds
	const int ALIVE_LED_DUTY_CYCLE_PERCENT = 10; // percent

	const uint8_t BUZZER_PIN = 4;
	const uint8_t BUZZER_OFF_STATE = LOW; // LOW is using a NPN transistor (preffered) to drive the buzzer or no transistor at all. HIGH if using a PNP.

	const uint8_t BATTERY_VOLTAGE_SENSOR_ANALOG_PIN = 1; // analog pin#
	const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000; // 10Kohm
	const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000; // 20Kohm

	// Discharge protection parameters
	const int DISCHARGE_PROTECTION_ALPHA = 0.02 * CalibratedSensor::MAX_EXP_ALPHA; // can try values closer to 0.01 if discharge protection kicks in during engine cranking.
	const int DISCHARGE_PROTECTION_LOW_CHARGE_TH = 11500; // 11.5V
	const int DISCHARGE_PROTECTION_HIGH_CHARGE_TH = 12000; // 12V

private:
};

#endif

