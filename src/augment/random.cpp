
#ifdef PLATFORM_TIRTOS

#define RAND_MAX UINT_MAX	// TODO match to definition in Util?

uint32 rand() {
	// High-level access to trng hw
	return Util_GetTRNG();	// TODO trng types match
}

#else

// use std libc rand()
#include "random.h"
// TODO is this libc or libstdc++

#endif

// TODO class and namespace to hide

// Utility routines, assuming platform only provides rand()

int randInt(int min, int max) {
	// Conventional implementation with slight flaws in randomness
	return rand() % max + min;
}

// depends on randInt
bool randBool() {
	return rand() < (RAND_MAX/2);	// integer division
}