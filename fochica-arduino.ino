#include "SensorVoltage.h"
#include "SensorVcc.h"
#include "SensorFreeRAM.h"
#include "ISensor.h"
#include "Manager.h"
#include "DebugStream.h"

const int SERIAL_BAUD = 9600;
const int BATTERY_VOLTAGE_SENSOR_PIN = 1;

void setup()
{
	Serial.begin(SERIAL_BAUD);
	DebugStream = &Serial;
	DebugStream->println("Start");

	SensorFreeRAM ram("SRAM");
	DebugStream->println(ram.getValueInt());

	SensorVcc vcc("Vcc");
	DebugStream->println(vcc.getValueFloat());

	SensorVoltage bat("Battery", BATTERY_VOLTAGE_SENSOR_PIN, 10000, 20000);
	DebugStream->println(bat.getValueFloat());

	DebugStream->println(ram.getValueInt());
}

void loop()
{

  /* add main program code here */

}
