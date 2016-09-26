#pragma once

#include <inttypes.h>

/*
 * Fundamental types, some must match os and radio
 */

// TODO OS wrapper

/*
 * MAC id of radio, unique system/unit identifier.
 * Content of sync msg to identify  master.
 * BT: 48-bits
 */
typedef uint64_t SystemID;	// lower 6 btyes






