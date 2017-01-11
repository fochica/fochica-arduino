// PacketType.h

#ifndef _PACKETTYPE_h
#define _PACKETTYPE_h

struct PacketType {
	enum e {
		Time = 't',
		TechnicalData = 'd',
		SensorData = 's',
		LogicalData = 'l',
		CalibrationParams = 'c',
		SeatOperation = 'o',
		SensorOperation = 'n'
	};
};

#endif

