// SeatOperation.h

#ifndef _SEATOPERATION_h
#define _SEATOPERATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct SeatOperation {
	enum e { StartCalibrationOccupied, StartCalibrationEmpty};
};

#endif

