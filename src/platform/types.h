#pragma once

#include <inttypes.h>

/*
 * Fundamental types between platform and SyncAgent.
 */

// TODO OS wrapper


/*
 * Ticks delivered by platform's free-running, circular clock/counter.
 * Called 'OS' but the platform need not have a true OS.
 *
 * On some platforms, only 24 bits valid.
 */
// TODO 24 bit NRF
typedef uint32_t OSTime;


/*
 * MAC id of radio, unique system/unit identifier.
 * Content of sync msg to identify  master.
 * BT: 48-bits
 */
typedef uint64_t SystemID;	// lower 6 btyes






