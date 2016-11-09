// DebugStream.h

#ifndef _DEBUGSTREAM_h
#define _DEBUGSTREAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// pointer to a stream that is used to send debug information
extern Stream * DebugStream;

#endif

