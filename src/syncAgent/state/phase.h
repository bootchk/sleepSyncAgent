
#pragma once

#include "../flashIndex.h"



/*
 * Simple enumeration of phase and subphases of a SyncPeriod.
 *
 * For debugging.
 * We can know what phase when asynchronous brownout occurs.
 *
 * Generally the name is <slot><subslot>
 */

enum class PhaseEnum {

	sleepAfterBoot,
	firstSyncPower,

	// Before loop
	Preamble,

	// Callback to app
	SyncPointCallback,

	// Maintain sync w/o radio power
	SleepEntireSyncPeriod,
	// Brief xmit to clique when pausing
	AbandonMastership,

	// Regular SyncPeriod, establish sync using radio

	// Sync slot preamble
	SyncChooseKind,
	SyncSkipSlot,

	// Sync slot Master
	SyncListenFirstHalf,
	SyncXmit,
	SyncXmitWorkSync,
	SyncListenSecondHalf,

	// Sync slot Slave
	SyncSlaveListen,

	// Fish slot
	// 9
	SleepTilFish,
	Fish,
	NoPowerToFish,

	// Merge slot
	// 12
	SleepTilMerge,
	Merge,

	// Sleep remainder of SyncPeriod
	// 14
	SleepRemainder,
	StartSlotSequence

};

namespace {
	PhaseEnum _phase;
}

class Phase {
public:
	static void set(PhaseEnum phase) {
		_phase = phase;
		// Debugging: write zero bit to Phase flag of UICR
		CustomFlash::writeBitAtIndex(PhasesDoneIndex, (unsigned int) phase);
	}
	static PhaseEnum get() { return _phase; }
};
