/*
Fochica� - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// IPersistentLog.h

#ifndef _IPERSISTENTLOG_h
#define _IPERSISTENTLOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <RTClib.h> // for DateTime

// interface of a persistent log, such an SD card
// supports only one open file at a time
class IPersistentLog
{
public:
	virtual boolean begin() = 0;
	virtual Print * open() = 0;
	virtual void close() = 0;

protected:
	void printDate(Print & out, const DateTime & d);
};

#endif

