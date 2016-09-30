
#include "../../config.h"
#include "ledLogger.h"

#if SYNC_AGENT_IS_LIBRARY==1

// platform implements

#else

// stubs

void LEDLogger::init() {}
void LEDLogger::toggleLEDs() {}
void LEDLogger::toggleLED(int ordinal) {}

#endif
