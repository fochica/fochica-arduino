// 
// 
// 

#include "TechnicalManager.h"

TechnicalManager::TechnicalManager()
{
	mVccSensor = mDeviceCurrentSensor = mCarBatteryVoltageSensor = mFreeRAMSensor = NULL;
}

void TechnicalManager::work(IClient * client)
{
	// send technical packet
	PacketTechnicalData packet;
	packet.vcc = mVccSensor ? mVccSensor->getValueInt() : 0;
	packet.deviceCurrent = mDeviceCurrentSensor ? mDeviceCurrentSensor->getValueInt() : 0;
	packet.carBatteryVoltage = mCarBatteryVoltageSensor ? mCarBatteryVoltageSensor->getValueInt() : 0;
	packet.freeRAM = mFreeRAMSensor ? mFreeRAMSensor->getValueInt() : 0;
	client->sendTechnicalData(packet);
}
