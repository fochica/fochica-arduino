// 
// 
// 

#include "EventHandlerConnectedSensorStateChange.h"
#include "SoundManager.h"

bool EventHandlerConnectedSensorStateChange::eventSensorStateChange(IEventHandlerState & state, seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState)
{
	if (state.isConnected()) {
		SoundManager::getInstance().playBeep(BeepType::SeatStateChange);
	}
	return true;
}
