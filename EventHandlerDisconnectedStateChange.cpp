// 
// 
// 

#include "EventHandlerDisconnectedStateChange.h"

bool EventHandlerDisconnectedStateChange::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if (state.isConnected()==false)
		SoundManager::getInstance().playBeep(BeepType::SeatStateChangeNoClient);
	return true;
}
