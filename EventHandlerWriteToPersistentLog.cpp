// 
// 
// 

#include "EventHandlerWriteToPersistentLog.h"
#include "PersistentLog.h"

bool EventHandlerWriteToPersistentLog::eventSeatStateChange(IEventHandlerState & state, seatCount_t seatId, SensorState::e lastState, SensorState::e newState)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Seat state change, seat: "));
			file->print(seatId);
			file->print(F(", state: "));
			file->print(lastState);
			file->print(F(" -> "));
			file->print(newState);
			file->println();
			PersistentLog->close();
		}
	}
	return true;
}

bool EventHandlerWriteToPersistentLog::eventSensorStateChange(IEventHandlerState & state, seatCount_t seatId, sensorCount_t sensorId, SensorState::e lastState, SensorState::e newState)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Seat sensor state change, seat: "));
			file->print(seatId);
			file->print(F(", sensor: "));
			file->print(sensorId);
			file->print(F(", state: "));
			file->print(lastState);
			file->print(F(" -> "));
			file->print(newState);
			file->println();
			PersistentLog->close();
		}
	}
	return true;
}

bool EventHandlerWriteToPersistentLog::eventClientConnectionChange(IEventHandlerState & state, bool isAdapterConnected)
{
	if (PersistentLog) {
		Print * file = PersistentLog->open();
		if (file) {
			file->print(F("Adapter connection change, connected: "));
			file->print(isAdapterConnected);
			file->println();
			PersistentLog->close();
		}
	}
	return true;
}
