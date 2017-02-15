// 
// 
// 

#include "EventHandlerDisconnectedStateChange.h"
#include "SoundManager.h"

bool EventHandlerDisconnectedStateChange::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if (state.isConnected() == false) {
		bool isPseudo = SensorState::Stabilizing == lastState && SensorState::SensorConflict == newState; // Stabilizing->SensorConflict is a pseudo change, caused by logic. not sensor readings.
		if (mIncludePseudoChanges || isPseudo == false)
			SoundManager::getInstance().playBeep(BeepType::SeatStateChangeNoClient);
	}
	return true;
}
