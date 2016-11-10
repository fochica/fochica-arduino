// 
// 
// 

#include "Manager.h"
#include "DebugStream.h"

bool Manager::receiveTime(const PacketTime & packet)
{
	DebugStream->print(F("Got time "));
	DebugStream->println(packet.utcTime);
}

