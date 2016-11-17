// 
// 
// 

#include "SensorQtouch.h"

SensorQtouch::SensorQtouch(const char * name, int analogReadPin, int analogReferencePin) : ISensor(name, SensorType::Capacitance)
{
	mReadPin = analogReadPin;
	mRefPin = analogReferencePin;
}

// returns values between 0 and 1024, usually lower than max.
// consider to add GAIN=10 to utilize int range best
int SensorQtouch::getValueInt()
{
	// First measurement  
	// this charges S&H and discharges read cap. then transfer happens to the read cap. new value is measured on S&H. gives adc1 which is subtracted from result!
	int adc1 = makeMeasurement(InToOut);

	// Second measurement - Do everything again with inverted ref / probe pins
	// this discharges S&H and charges the read cap. then transfer happens to the S&H cap. new value is measured on S&H. gives adc2 which is added to the result!
	int adc2 = makeMeasurement(OutToIn);

	return adc2 - adc1; // return conversion result
}

float SensorQtouch::getValueFloat()
{
	return getValueInt();
}

int SensorQtouch::getSamplingTime()
{
	return 2*(CHARGE_DELAY + TRANSFER_DELAY + ISensor::DEFAULT_ADC_SAMPLING_TIME);
}

// Possibly AVR specific
// cross platform issue
// some implementations online used to read from the reference pin, but my tests have shown this is inferior. Still, keep this mode as an option (altRead)
int SensorQtouch::makeMeasurement(bool dir, bool altRead)
{
	// config pins as push-pull output
	pinMode(analogInputToDigitalPin(mReadPin), OUTPUT);
	pinMode(analogInputToDigitalPin(mRefPin), OUTPUT);

	if (dir == InToOut)
	{
		// set partner high to charge the s&h cap and pin low to discharge touch probe cap
		digitalWrite(analogInputToDigitalPin(mReadPin), LOW);
		digitalWrite(analogInputToDigitalPin(mRefPin), HIGH);
	}
	else // OutToIn
	{
		// set read pin high to charge the touch probe and partner low to discharge s&h cap
		digitalWrite(analogInputToDigitalPin(mReadPin), HIGH);
		digitalWrite(analogInputToDigitalPin(mRefPin), LOW);
	}

	// charge/discharge s&h cap by connecting it to partner (direction depends on partner state)
	ADMUX = (ADMUX & ~ADMUX_MASK) | (mRefPin & ADMUX_MASK); // note, some arduino MCU can have MUX4
														   // wait for the charge to transfer
	delayMicroseconds(CHARGE_DELAY); // wait for the touch probe and the s&h cap to be fully charged/discharged 

	// config pins input
	// disable the internal pullup to make the ports high Z 
	pinMode(analogInputToDigitalPin(mReadPin), INPUT);
	pinMode(analogInputToDigitalPin(mRefPin), INPUT);

	// connect touch probe cap to s&h cap to transfer the charge (direction depends on who was charged)
	ADMUX = (ADMUX & ~ADMUX_MASK) | (mReadPin & ADMUX_MASK); // some arduino MCU can have MUX4
	delayMicroseconds(TRANSFER_DELAY); // wait for charge to be transfered  

	// measure
	if (altRead)
		return analogRead(mRefPin); // why use reference-pin in one pass and use read-pin both times? this methond has a similar span, but about 400units lower... not sure how this works, against documentation.
	return analogRead(mReadPin); // my change
}
