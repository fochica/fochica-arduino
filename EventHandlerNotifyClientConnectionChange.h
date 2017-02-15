// EventHandlerNotifyClientConnectionChange.h

#ifndef _EVENTHANDLERNOTIFYCLIENTCONNECTIONCHANGE_h
#define _EVENTHANDLERNOTIFYCLIENTCONNECTIONCHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when a seat state changes and there is a client (or clients) connected.
// This indicates an informational messages to the user that system is in an optimal state.
class EventHandlerNotifyClientConnectionChange : public IEventHandler
{
public:
	bool eventClientConnectionChange(IEventHandlerState &state, bool isAdapterConnected);
};


#endif

