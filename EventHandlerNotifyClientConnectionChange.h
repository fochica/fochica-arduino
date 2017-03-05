// EventHandlerNotifyClientConnectionChange.h

#ifndef _EVENTHANDLERNOTIFYCLIENTCONNECTIONCHANGE_h
#define _EVENTHANDLERNOTIFYCLIENTCONNECTIONCHANGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IEventHandler.h"

// Makes a sound notification when an adapter to a client device connects or disconnects.
// This indicates an informational messages to the user and helps troubleshoot connectivity issues.
class EventHandlerNotifyClientConnectionChange : public IEventHandler
{
public:
	bool eventClientConnectionChange(IEventHandlerState &state, bool isAdapterConnected);
};


#endif

