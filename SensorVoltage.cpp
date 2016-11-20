// 
// 
// 

#include "SensorVoltage.h"

SensorVoltage::SensorVoltage(const char * name, int analogPin, long resistorToGround, long resistorToVoltage) : ISensor(name, SensorType::Voltage)
{
	mPin = analogPin;
	mDivider = (resistorToGround + resistorToVoltage) / resistorToGround;
}

// returns value in mV. don't use as-it-is now for measuring voltage over 32V
sensorVal_t SensorVoltage::getValueInt()
{
	return getValueFloat() * 1000;
}

float SensorVoltage::getValueFloat()
{
	int v = analogRead(mPin);
	return (float)mDivider*ISensor::DEFAULT_ADC_ANALOG_REFERENCE*v / ISensor::MAX_ADC_VALUE;
}

int SensorVoltage::getSamplingTime()
{
	return ISensor::DEFAULT_ADC_SAMPLING_TIME;
}
