// SensorMisc.h

#ifndef _SENSORMISC_h
#define _SENSORMISC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef uint8_t seatCount_t;
typedef uint8_t sensorCount_t;

struct SensorLocation {
	enum e { Virtual, UnderSeat, Chest, Above };
};

struct SensorState { // relevant for seat sensors
	enum e {
		None, // no sensors to form an opinion for aggregated sensor or initial state for a single sensor
		Occupied,
		Empty,
		NotCalibrated,
		Disabled,
		// aggregated states
		Stabilizing, // disagreement but still in time window to be resolved.
		SensorConflict, // disagreement past time window
		Count
	};
};

struct SensorActivityMode {
	enum e { Active, Deactivated, Disabled }; // Deactivated: don't take part in aggregated state, Disabled: don't even take readings
};

#endif

