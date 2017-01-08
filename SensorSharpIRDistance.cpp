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
	int mm = 240 / v; // convert to distance (mm) per device spec, const is not really important, mapping depends on reflectiveness of the object. can even return [0-1023] analog reading as-is and let the calibrator tidy this data.
	// consider to clamp the distance per device spec [100mm, 800mm]
	return mm;
}

float SensorSharpIRDistance::getValueFloat()
{
	return getValueInt();
}

int SensorSharpIRDistance::getSamplingTime()
{
	return ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
