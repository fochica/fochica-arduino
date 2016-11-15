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

int SensorDigital::getValueInt()
{
	return digitalRead(mPin);
}

float SensorDigital::getValueFloat()
{
	return getValueInt();
}

int SensorDigital::getSamplingTime()
{
	return DEFAULT_DIGITAL_SAMPLING_TIME;
}
