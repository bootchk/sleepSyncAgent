
#pragma once

#include "slot.h"

/*
 * Combined work/sync slot.
 * Sync period has one sync slot and no separate work slot.
 * WorkSync message carries sync.
 *
 * Each Clique is on a Schedule that includes a sync slot.
 * See general notes in Schedule.
 *
 *Cases:
 * a) A clique's Master xmits sync FROM its sync slot.
 *
 * b) A Merger xmits sync FROM a sleeping slot INTO another clique's sync slot.
 *
 * c) One clique's sleeping slot may coincidentally be near in time to another clique's sleeping slot, or drift into each other.
 *
 * d) A clique Master or Slave may xmit work from this slot, carrying sync info
 *
 * Cases b-d have contention (two syncs in same interval of a sync slot):
 * - b: merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 * - c: two cliques may unwittingly be in sync (two Masters xmitting in same interval)
 * - d: a Master xmitting sync and a Slave xmitting work
 *
 * Slot is divided into three subslots:
 * - listen
 * - xmit
 * - listen
 * Any xmit is in the center of the slot (centered in the middle subslot.)
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 */


class SyncWorkSlot: Slot {

public:
	void perform();

	/*
	 * Three behaviours of slot:
	 * - Master
	 * - Slave listening
	 * - Master or Slave conveying work
	 * 
	 * These are not local because they refer to superclass dispatcher i.e. this
	 */
	void doSendingWorkSyncWorkSlot();
	void doMasterSyncWorkSlot();
	void doSlaveSyncWorkSlot();

	// Utility
	bool doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)());
	
	
	
	// override superclass message handlers
	bool doMasterSyncMsg(SyncMessage* msg);
	bool doMergeSyncMsg(SyncMessage* msg);
	bool doAbandonMastershipMsg(SyncMessage* msg);
	bool doWorkMsg(SyncMessage* msg);
};
