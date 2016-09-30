
#include "../platform/platformAbstractionForSync.h"	// rand()


// Wrapper to platform provided std lib functions

// depends on stdlib rand()
uint16_t randUnsignedInt16(uint16_t min, uint16_t max);

// depends on rand()
bool randBool();
