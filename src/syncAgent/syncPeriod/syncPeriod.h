
#pragma once


/*
 * Sequence of Slots.
 * Slots vary according to state of synchronization.
 */

// Abstract Base Class
class SyncPeriod {
private:
	// Owns Slot instances
public:
	/*
	 * One cycle executing the sequence.
	 * Defines slot order and how state determines which slots executed.
	 */
	static void doSlotSequence();
};


/*
 * Having separate Sync and Work slots.
 * This eases issues of contention (work is more reliable.)
 * But at the cost of more power: 3 active slots per SyncPeriod.
 */
class SimpleSyncPeriod : SyncPeriod {

public:
	static void doSlotSequence();
};

/*
 * Having combined SyncWork slot.
 * WorkSync message carries sync info.
 * More contention in the combined slot.
 * Less power: 2 active slots per SyncPeriod.
 * Work must be rare, else it floods and hinders sync?
 */
class CombinedSyncPeriod : SyncPeriod {

public:
	static void doSlotSequence();
};
