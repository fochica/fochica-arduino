// 
// 
// 

#include "RNGUtils.h"
#include "DebugStream.h"

unsigned long RNGUtils::generateEntropyWithAnalogInputs()
{
	unsigned long entropy = millis() + micros(); // this alone might not be a good source of entropy because seeding might happen at the same time each boot. consider to add RTC value if available.
	if (DebugStream) {
		DebugStream->print("Entropy value: ");
		DebugStream->println(entropy);
	}

	for (int pin = 0; pin <= ANALOGS_FOR_SEED; pin++)
		entropy += analogRead(pin);
	if (DebugStream) {
		DebugStream->print("Entropy value: ");
		DebugStream->println(entropy);
	}

	return entropy;
}

void RNGUtils::seedWithAnalogInputs()
{
	randomSeed(generateEntropyWithAnalogInputs());
}

// get random gaussian distributed value
// mean=0, sigma=1
double RNGUtils::getGaussian()
{
	// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
	double u, v, S;

	do {
		// The random() function computes a sequence of pseudo - random integers in the range of 0 to \c RANDOM_MAX
		// Arduino's random actually uses this stdlin random function
		u = (double)random() / RANDOM_MAX;
		v = (double)random() / RANDOM_MAX;
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
	return random();
}
