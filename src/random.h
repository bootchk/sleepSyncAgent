
#include <cstdlib>


// Wrapper to platform provided std lib functions

// depends on stdlib rand()
int randInt(int min, int max) {
	// Conventional implementation with slight flaws in randomness
	return rand() % max + min;
}

// depends on randInt
bool randBool() {
	return randInt(0,1);
}
