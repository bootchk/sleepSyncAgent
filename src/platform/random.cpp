
#ifdef PLATFORM_TIRTOS

uint32 rant() {
	// TODO TRNG
}

#else

// use stdlib rand()
#include "random.h"

#endif


// Utility routines, assuming platform only provides rand()

int randInt(int min, int max) {
	// Conventional implementation with slight flaws in randomness
	return rand() % max + min;
}

// depends on randInt
bool randBool() {
	return rand() < (RAND_MAX/2);
}
