// 
// 
// 

#include "EventHandlerConnectedStateChange.h"
#include "SoundManager.h"

bool EventHandlerConnectedStateChange::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if (state.isConnected()) {
		bool isPseudo = SensorState::Stabilizing == lastState && SensorState::SensorConflict == newState; // Stabilizing->SensorConflict is a pseudo change, caused by logic. not sensor readings.
		if(mIncludePseudoChanges || isPseudo==false)
			SoundManager::getInstance().playBeep(BeepType::SeatStateChange);
	}
	return true;
}
