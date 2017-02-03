// 
// 
// 

#include "IEventHandlerState.h"

bool IEventHandlerState::isAnySeatNotEmpty()
{
	seatCount_t seats = getSeatCount();
	for (seatCount_t i = 0; i < seats; i++) {
		if (getSeatState(i) != SensorState::Empty)
			return true;
	}
	return false;

}
