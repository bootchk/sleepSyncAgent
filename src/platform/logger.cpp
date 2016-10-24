
#include "logger.h"

// stubs

void initLogging() {}
void log(const char* aString) {}
void logLongLong(uint64_t) {}
int SEGGER_RTT_printf(unsigned, const char* formatString, ...) { return 1; }
