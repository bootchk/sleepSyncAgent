
#include "logger.h"

// stubs

void initLogging() {}
void log(const char* aString) { (void) aString; }
void logLongLong(uint64_t value) { (void) value; }
int SEGGER_RTT_printf(unsigned device, const char* formatString, ...) {
	(void) device;
	(void) formatString;
	return 1; }
