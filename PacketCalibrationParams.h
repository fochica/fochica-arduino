/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PacketCalibrationParams.h

#ifndef _PACKETCALIBRATIONPARAMS_h
#define _PACKETCALIBRATIONPARAMS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

struct PacketCalibrationParams {
	uint8_t sensorId;
	uint8_t seatId;
	uint16_t triggerHigh;
	uint16_t triggerLow;
	uint16_t lowPassAlpha; // in milli units
	uint8_t highIsOccupied; // boolean
	uint8_t calibrationState; // CalibrationState::e
};

#endif

