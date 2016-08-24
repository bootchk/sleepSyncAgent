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
/*
 * Time offset.  2-bytes, 16-bits, 128k
 * Content of sync msg to adjust sync.
 */
typedef uint16_t SyncOffset;
/*
 * Used in scheduling to count slots and periods.
 */
typedef short ScheduleCount;
