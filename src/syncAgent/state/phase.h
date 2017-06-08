
#pragma once

/*
 * Simple enumeration of phase and subphases of a SyncPeriod.
 *
 * For debugging.
 * We can know what phase when asynchronous brownout occurs.
 *
 * Generally the name is <slot><subslot>
 */

enum class Phase {
	Preamble,
	SyncListenFirstHalf,
	SyncXmit,
	SyncListenSecondHalf
};
