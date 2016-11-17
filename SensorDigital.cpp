// 
// 
// 

#include "SensorDigital.h"

SensorDigital::SensorDigital(const char * name, uint8_t pin, int8_t pinMode) : ISensor(name, SensorType::OnOff)
{
	mPin = pin;
	mPinMode = pinMode;
}

void SensorDigital::begin()
{
	if(mPinMode!=-1)
		pinMode(mPin, mPinMode);
}

// value is 0 or 1*GAIN
int SensorDigital::getValueInt()
{
	return digitalRead(mPin)*GAIN;
}

// value is 0 or 1
float SensorDigital::getValueFloat()
{
	return getValueInt();
}

int SensorDigital::getSamplingTime()
{
	return DEFAULT_DIGITAL_SAMPLING_TIME;
}
