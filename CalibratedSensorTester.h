/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// CalibratedSensorTester.h

#ifndef _CALIBRATEDSENSORTESTER_h
#define _CALIBRATEDSENSORTESTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CalibratedSensor.h"

// This class is used for testing of th calibrated sensor logic
// This is not used during normal operation but only during development and testing
class CalibratedSensorTester
{
public:
	bool runTests();

private:
	void getAndPrintSample(CalibratedSensor & cs);
};


#endif

