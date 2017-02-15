// 
// 
// 

#include "EventHandlerNotifyClientConnectionChange.h"
#include "SoundManager.h"

// Makes a sound notification when an adapter to a client device connects or disconnects.
bool EventHandlerNotifyClientConnectionChange::eventClientConnectionChange(IEventHandlerState & state, bool isAdapterConnected)
{
	SoundManager::getInstance().playBeep(isAdapterConnected ? BeepType::ClientConnected : BeepType::ClientDisconnected);
	return true;
}
