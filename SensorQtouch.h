// SensorQtouch.h

#ifndef _SENSORQTOUCH_h
#define _SENSORQTOUCH_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "ISensor.h"

// Sensor that uses capacitance to sense what is near the sensor
// http://www.atmel.com/images/doc8497.pdf - Atmel AVR3001: QTouchADC Measurement and Tuning
// Usage example in http://www.electrobob.com/sit-up/ project
// Requires two pins. The secondary pin (reference) can be a main pin of another sensor instance.
// The secondary reference pin is special for this sensor logic and is not to be confused with MCU ADC reference
class SensorQtouch : public ISensor
{
public:
	SensorQtouch(const char * name, int analogReadPin, int analogReferencePin);
	int getValueInt(); // returns V*[internal coeff], higher when capacitance is higher
	float getValueFloat();
	int getSamplingTime();

private:
	int mReadPin;
	int mRefPin;

	enum TransferDirection {
		InToOut, // charge internal cap, then share charge with external cap
		OutToIn // charge external cap, then share charge with internal cap
	};

	const int CHARGE_DELAY = 10; // time it takes for the capacitor to get charged/discharged in microseconds
	const int TRANSFER_DELAY = 5; // time it takes for the capacitors to exchange charge
	const byte ADMUX_MASK = 0b00011111; // mask the mux bits in the ADMUX register - including for MCU with MUX4

	int makeMeasurement(bool dir, bool altRead = false);
};

#endif

