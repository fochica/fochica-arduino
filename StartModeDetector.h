// StartModeDetector.h

#ifndef _STARTMODEDETECTOR_h
#define _STARTMODEDETECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

struct StartMode {
	enum e { Unknown, // unable to detect or detection not performed yet
		Restart, // restarted/rebooted via software or through use of reboot/reset pin/button
		PowerCycle, // power disconnected and reconnected in sequence (2-10s)
		PowerOn // power connected after prolonged off state (>20sec)
	};
};

// This logic in conjunction with dedicated circuitry on the board will detect the type of start that the device is going through
// The circuit is composed of an RC cicrcuit with a discharge capacitor
class StartModeDetector
{
public:
	StartModeDetector(uint8_t chargePin, uint8_t measurePin, int thRestart, int thPowerCycle);

	void begin(); // must be called ASAP after boot
	void debug();

	StartMode::e getMode() {
		return mMode;
	}

protected:
private:
	uint8_t mChargePin, mMeasurePin; // can be the same pin
	int mThresholdRestart, mThresholdPowerCycle;

	StartMode::e mMode;
	int mMeasurement;
};


#endif

