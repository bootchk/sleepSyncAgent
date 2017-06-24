
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

	// Before loop
	Preamble,

	// Callback to app
	SyncPointCallback,

	// Maintain sync w/o radio power
	SleepEntireSyncPeriod,
	// Brief xmit to clique when pausing
	AbandonMastership,

	// Regular SyncPeriod, establish sync using radio

	// Sync slot Master
	// 4-8
	SyncListenFirstHalf,
	SyncXmit,
	SyncXmitWorkSync,
	SyncListenSecondHalf,
	// Sync slot Slave
	SyncSlaveListen,

	// Fish slot
	// 9, 10
	SleepTilFish,
	Fish,

	// Merge slot
	// 11, 12
	SleepTilMerge,
	Merge,

	// Sleep remainder of SyncPeriod
	// 13
	SleepRemainder,
	StartSlotSequence

};
