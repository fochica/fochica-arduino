// DischargeProtectionManager.h

#ifndef _DISCHARGEPROTECTIONMANAGER_h
#define _DISCHARGEPROTECTIONMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DebugStream.h"
#include "PersistentLog.h"
#include "CalibratedSensor.h"
#include "SoundManager.h"

// This class monitors a calibrated sensor of battery voltage and sends a HIGH signal on some been as long as we want to keep using power.
// Once the monitor detects that the voltage is in "low state" it stops emitting the "keep-live" signal causing the discharge protection circuit to stop power to the device.
// If battery voltage sensing circuit is not working, this can cause us to shutdown when there is no need to.
// If the discharge protection circuit is not working, this can cause us to keep running even though we are in what seems to be a "low state".
class DischargeProtectionManager
{
public:
	DischargeProtectionManager(CalibratedSensor & lowChargeSensor, uint8_t keepAlivePin);
	void begin();
	void work();

private:
	CalibratedSensor & mLowChargeSensor;
	uint8_t mKeepAlivePin;
	boolean mShutdownDone;
};

#endif

