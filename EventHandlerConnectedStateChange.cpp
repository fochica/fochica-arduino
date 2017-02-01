// 
// 
// 

#include "EventHandlerConnectedStateChange.h"

bool EventHandlerConnectedStateChange::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if(state.isConnected())
		SoundManager::getInstance().playBeep(BeepType::SeatStateChange);
	return true;
}
