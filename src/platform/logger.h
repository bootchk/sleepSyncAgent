
#pragma once

#include <inttypes.h>

void initLogging();
void log(const char* aString);
void logLongLong(uint64_t);
// Until I wrap it, use original prototype
//extern "C" {

// !!! Note this does not support ll conversion code
int SEGGER_RTT_printf(unsigned, const char* formatString, ...);
