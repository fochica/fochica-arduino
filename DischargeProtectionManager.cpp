// 
// 
// 

#include "DischargeProtectionManager.h"

DischargeProtectionManager::DischargeProtectionManager(CalibratedSensor & lowChargeSensor, uint8_t keepAlivePin) : mLowChargeSensor(lowChargeSensor)
{
	mKeepAlivePin = keepAlivePin;
	mShutdownDone = false;
}

void DischargeProtectionManager::begin()
{
	// start keep-alive signal so we don't loose power
	pinMode(mKeepAlivePin, OUTPUT);
	digitalWrite(mKeepAlivePin, HIGH);
}

void DischargeProtectionManager::work()
{
	CalibratedSensorState::e state = mLowChargeSensor.getValue();
	if (DebugStream) {
		DebugStream->print(F("DischargeProtectionManager, state="));
		DebugStream->print(state);
		DebugStream->println();
	}

	if (state == CalibratedSensorState::A && mShutdownDone==false) { // if low state
		if (DebugStream) {
			DebugStream->print(F("Detecting low charge state and terminating keep-alive signal at pin="));
			DebugStream->print(mKeepAlivePin);
			DebugStream->println();
		}
		if (PersistentLog) {
			Print * f = PersistentLog->open();
			if (f) {
				f->print(F("Detecting low charge state and terminating keep-alive signal at pin="));
				f->print(mKeepAlivePin);
				f->println();
				PersistentLog->close();
			}
		}
		SoundManager::getInstance().playBeep(BeepType::DischargeProtectionShutdown);
		digitalWrite(mKeepAlivePin, LOW);
		mShutdownDone = true; // prevent these notifications from hapenning if power continues while state is low (meaning we shouldn't shutdown the power supply)
	}
}
