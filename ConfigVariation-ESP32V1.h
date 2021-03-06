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

#ifndef ESP32
#error This configuration is for ESP32 build settings
#endif

#include "ConfigVariationESP32Base.h"
#include "SensorCapacitivePressure1PinESP32.h"
#include "EventHandlerClientConnectionStatePin.h"

#define ADC_6db_REFERENCE 1.9f // not accurate, https://esp32.com/viewtopic.php?f=12&t=1045, TODO

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Configuration variations for an ESP32 build
// Internal BLE module, 2 seat, 2 sensors
class ConfigVariation : public ConfigVariationESP32Base
{
public:
	ConfigVariation();

	virtual uint8_t getBatteryVoltageSensorAnalogPin() { return BATTERY_VOLTAGE_SENSOR_ANALOG_PIN; }
	virtual long getBatteryVoltageSensorResistorToGroundValue() { return BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND; }
	virtual long getBatteryVoltageSensorResistorToVoltageValue() { return BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE; }

	virtual uint8_t getVccVoltageSensorAnalogPin() { return VCC_VOLTAGE_SENSOR_ANALOG_PIN; }
	virtual long getVccVoltageSensorResistorToGroundValue() { return VCC_VOLTAGE_SENSOR_RESISTOR_GROUND; }
	virtual long getVccVoltageSensorResistorToVoltageValue() { return VCC_VOLTAGE_SENSOR_RESISTOR_VOLTAGE; }

	virtual ISensor * getVccSensor() { 
		analogSetPinAttenuation(getVccVoltageSensorAnalogPin(), ADC_6db); // try to work in a more accurate range
		return new SensorVoltage(getVccVoltageSensorAnalogPin(), getVccVoltageSensorResistorToGroundValue(), getVccVoltageSensorResistorToVoltageValue(), ADC_6db_REFERENCE);
	};
	virtual ISensor * getBatterySensor() { 
		analogSetPinAttenuation(getBatteryVoltageSensorAnalogPin(), ADC_6db); // try to work in a more accurate range
		return new SensorVoltage(getBatteryVoltageSensorAnalogPin(), getBatteryVoltageSensorResistorToGroundValue(), getBatteryVoltageSensorResistorToVoltageValue(), ADC_6db_REFERENCE);
	};
	virtual StartModeDetector * getStartModeDetector() { 
		return new StartModeDetector(START_MODE_CHARGE_PIN, START_MODE_MEASURE_PIN, START_MODE_RESTART_TH, START_MODE_POWER_CYCLE_TH);
	};

	virtual uint8_t getBuzzerPin() { return BUZZER_PIN; }

	virtual void registerSensors(SensorManager & sm);

	// event handlers
	virtual int getEventHandlerCount();
	virtual void registerEventHandlers(IEventHandlerManager & handlerManager, CalibratedSensor & carEngineState);

	virtual void loop();

protected:

private:
	// override
	const uint8_t BUZZER_PIN = 14;

	const uint8_t BATTERY_VOLTAGE_SENSOR_ANALOG_PIN = 33; // analog pin#
	const long BATTERY_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000; // 10Kohm
	const long BATTERY_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000; // 20Kohm

	const uint8_t VCC_VOLTAGE_SENSOR_ANALOG_PIN = 32; // analog pin#
	const long VCC_VOLTAGE_SENSOR_RESISTOR_GROUND = 10000; // 10Kohm
	const long VCC_VOLTAGE_SENSOR_RESISTOR_VOLTAGE = 20000; // 20Kohm

	// pressure sensors
	const int CAPACITIVE_PRESSURE_SENSOR_PIN = 25;
	const int CAPACITIVE_PRESSURE_SENSOR2_PIN = 27;

	// start mode
	const int START_MODE_CHARGE_PIN = 21;
	const int START_MODE_MEASURE_PIN = 35;
	const int START_MODE_RESTART_TH = 1000;
	const int START_MODE_POWER_CYCLE_TH = 50;
};

ConfigVariation::ConfigVariation()
{

}

SensorCapacitivePressure1PinESP32 * capPressureDebug;

void ConfigVariation::registerSensors(SensorManager & sm)
{
	SensorCapacitivePressure1PinESP32 * capPressure = new SensorCapacitivePressure1PinESP32(CAPACITIVE_PRESSURE_SENSOR_PIN);
	SensorCapacitivePressure1PinESP32 * capPressure2 = new SensorCapacitivePressure1PinESP32(CAPACITIVE_PRESSURE_SENSOR2_PIN);
	capPressureDebug = capPressure;

	sm.setSeatCount(2);
	sm.setSensorCount(2);

	capPressure->begin();
	sm.addSensor(0, SensorLocation::UnderSeat, capPressure);
	capPressure2->begin();
	sm.addSensor(1, SensorLocation::UnderSeat, capPressure2);
}

int ConfigVariation::getEventHandlerCount()
{
	return ConfigVariationESP32Base::getEventHandlerCount()+1;
}

void ConfigVariation::registerEventHandlers(IEventHandlerManager & handlerManager, CalibratedSensor & carEngineState)
{
	ConfigVariationESP32Base::registerEventHandlers(handlerManager, carEngineState);

	// makes a sound notification when an adapter to a client device connects or disconnects.
	EventHandlerClientConnectionStatePin * ehClientConnectionStatePin = new EventHandlerClientConnectionStatePin(LED_BUILTIN);
	ehClientConnectionStatePin->begin();
	handlerManager.addEventHandler(ehClientConnectionStatePin);
}

void ConfigVariation::loop()
{
	/*if (DebugStream) {
		DebugStream->print("Cap: ");
		DebugStream->println(capPressureDebug->getValueInt());
	}*/
}

#else
#error Only one ConfigVariation can be included
#endif