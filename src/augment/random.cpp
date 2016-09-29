
#include "random.h"	// This is our random.h, not from libstdc++


// FUTURE class and namespace to hide

// Utility routines, assuming platform only provides rand()

int randInt(int min, int max) {
	// Conventional implementation with slight flaws in randomness
	return rand() % max + min;
}

// depends on randInt
bool randBool() {
	return rand() < (RAND_MAX/2);	// integer division
}
