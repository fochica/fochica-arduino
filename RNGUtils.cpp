/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 
// 
// 

#include "RNGUtils.h"
#include "DebugStream.h"

#ifndef RANDOM_MAX
#define	RANDOM_MAX 0x7FFFFFFF
#endif

unsigned long RNGUtils::generateEntropy()
{
	unsigned long entropy = millis() + micros(); // this alone might not be a good source of entropy because seeding might happen at the same time each boot. consider to add RTC value if available.
	if (DebugStream) {
		DebugStream->print(F("Entropy value: "));
		DebugStream->println(entropy);
	}

#ifdef ESP32
	// in prefered mode of operation, this will not be used and the hardware RNG will be used as long as BT or Wifi is initialized
	entropy += xthal_get_ccount();
	if (DebugStream) {
		DebugStream->print(F("Entropy value 2: "));
		DebugStream->println(entropy);
	}
#endif

#ifdef __AVR__
	static const int ANALOGS_FOR_SEED = 5;

	for (int pin = 0; pin <= ANALOGS_FOR_SEED; pin++)
		entropy += analogRead(pin);
	if (DebugStream) {
		DebugStream->print(F("Entropy value 2: "));
		DebugStream->println(entropy);
	}
#endif

	return entropy;
}

void RNGUtils::seed()
{
	randomSeed(generateEntropy());
}

// get random gaussian distributed value
// mean=0, sigma=1
double RNGUtils::getGaussian()
{
	// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
	double u, v, S;

	do {
		// The random() function computes a sequence of pseudo - random integers in the range of 0 to 1 [0,1)
		// Use Arduino's random for cross platform compatibility
		u = (double)random(RANDOM_MAX) / RANDOM_MAX;
		v = (double)random(RANDOM_MAX) / RANDOM_MAX;
		u = 2.0*u - 1; // shift to [-1,1] range
		v = 2.0*v - 1; // shift to [-1,1] range
		S = u*u + v*v; // r^2
	} while (S >= 1); // perform rejection sampling

	// we will only use z1 here. consider to save z2 for next request...
	return u * sqrt(-2.0 * log(S) / S);
}

long RNGUtils::getLong(long min, long max)
{
	return random(min, max);
}

long RNGUtils::getLong(long max)
{
	return random(max);
}

long RNGUtils::getLong()
{
	return random(RANDOM_MAX);
}
