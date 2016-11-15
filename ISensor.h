// ISensor.h

#ifndef _ISENSOR_h
#define _ISENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// put the enum in a scope that a struct creates
// http://stackoverflow.com/questions/482745/namespaces-for-enum-types-best-practices
struct SensorType {
	enum e { Vcc, Voltage, RAM, Capacitance, OnOff };
};

class ISensor
{
 public:
	 ISensor(const char * name, SensorType::e type);

	 virtual void begin() {}

	 const char * getName() { return mName.c_str(); }
	 SensorType::e getType() { return mType; }
	 
	 virtual int getValueInt() =0; // read sensor value
	 virtual float getValueFloat() = 0; // read sensor value
	 virtual int getSamplingTime() = 0; // get the time it takes to read a value (estimate) in microseconds

	 const int DEFAULT_ADC_SAMPLING_TIME=100; // microseconds
	 const int DEFAULT_DIGITAL_SAMPLING_TIME = 1; // microseconds
	 const float DEFAULT_ADC_ANALOG_REFERENCE = 5;
	 const int MAX_ADC_VALUE = 1023; // including this value

private:
	String mName;
	SensorType::e mType;

};

#endif

