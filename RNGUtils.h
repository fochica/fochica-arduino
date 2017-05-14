/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// RNGUtils.h

#ifndef _RNGUTILS_h
#define _RNGUTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class RNGUtils
{
public:
	static unsigned long generateEntropyWithAnalogInputs();
	static void seedWithAnalogInputs();
	static double getGaussian();
	static long getLong(long min, long max);
	static long getLong(long max);
	static long getLong();

private:
	static const int ANALOGS_FOR_SEED = 5;
};

#endif

