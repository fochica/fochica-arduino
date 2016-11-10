// Manager.h

#ifndef _MANAGER_h
#define _MANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "IServer.h"

struct SensorLocation {
	enum e { Virtual, UnderSeat, Chest, Above };
};

// This main class is a singleton
// http://stackoverflow.com/questions/8811988/can-i-return-a-reference-to-a-static-class-singleton-instance-within-that-clas
// http://stackoverflow.com/questions/13047526/difference-between-singleton-implemention-using-pointer-and-using-static-object
class Manager : public IServer
{
private:
	// protect constructors to prevent singleton modification
	Manager() { }
	Manager(const Manager &rhs);
	Manager & operator=(const Manager &rhs);

	bool receiveTime(const PacketTime& packet);

public:
	static Manager& getInstance() {
		static Manager singleton;
		return singleton;
	}

};

#endif

