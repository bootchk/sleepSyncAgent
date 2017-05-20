
#pragma once


/*
 * Sequence of Slots.
 * Slots performed varies according to state of synchronization.
 *
 * Owns Slot instances.
 * One method, to execute one cycle through the slots.
 * Defines slot order and how state determines which slots performed.
 */


/*
 * Having separate Sync and Work slots.
 * This eases issues of contention (work is more reliable.)
 * But at the cost of more power: 3 active slots per SyncPeriod.
 */
class SimpleSyncPeriod{

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
class CombinedSyncPeriod{

public:
	static void doSlotSequence(PowerManager* powerManager);
};
