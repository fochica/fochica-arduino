// 
// 
// 

#include "StartModeDetector.h"
#include "DebugStream.h"

StartModeDetector::StartModeDetector(uint8_t chargePin, uint8_t measurePin, int thRestart, int thPowerCycle) : mChargePin(chargePin), mMeasurePin(measurePin), mThresholdRestart(thRestart), mThresholdPowerCycle(thPowerCycle), mMode(StartMode::Unknown), mMeasurement(0)
{
}

void StartModeDetector::begin()
{
	// measure charge
	mMeasurement = analogRead(mMeasurePin);

	// start charging
	pinMode(mChargePin, OUTPUT);
	digitalWrite(mChargePin, HIGH);

	// parse result
	if (mMeasurement > mThresholdRestart)
		mMode = StartMode::Restart;
	else if (mMeasurement > mThresholdPowerCycle)
		mMode = StartMode::PowerCycle;
	else
		mMode = StartMode::PowerOn;
}

void StartModeDetector::debug()
{
	if (DebugStream!=NULL) {
		DebugStream->print(F("StartModeDetector, val: "));
		DebugStream->print(mMeasurement);
		DebugStream->print(F(", mode: "));
		DebugStream->println(mMode);
	}
}
