// 
// 
// 

#include "SensorSharpIRDistance.h"

SensorSharpIRDistance::SensorSharpIRDistance(const char * name, uint8_t analogReadPin) : ISensor(name, SensorType::IRDistance)
{
	mReadPin = analogReadPin;
}

sensorVal_t SensorSharpIRDistance::getValueInt()
{
	int k = analogRead(mReadPin);
	float v = (float)k*ISensor::DEFAULT_ADC_ANALOG_REFERENCE / 1024; // get voltage
	return 240/v; // convert to distance per device spec
}

float SensorSharpIRDistance::getValueFloat()
{
	return getValueInt();
}

int SensorSharpIRDistance::getSamplingTime()
{
	return ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
