// 
// 
// 

#include "SensorDigital.h"

SensorDigital::SensorDigital(const char * name, int pin) : ISensor(name, SensorType::OnOff)
{
	mPin = pin;
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
