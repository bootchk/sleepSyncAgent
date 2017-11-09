#pragma once

/*
 * Knows essential behaviours of sync algorithm.
 *
 * Collaborates (called by) SyncSlot and other variants of SyncSlot.
 * But does not depend on Slot.
 *
 * History: formerly part of SyncSlot.
 */

#include "../message/message.h"



class SyncBehaviour {
public:

	/*
	 * Cases for sync messages in sync slot:
	 * 1. my cliques current master (usual) (MasterSync)
	 * 2. other clique master happened to start schedule coincident with my schedule (MasterSync or WorkSync)
	 * 3. other clique clock drifts so schedules coincide (MasterSync, MergeSync, or WorkSync)
	 * 4. member (master or slave) of other, better clique fished, caught my clique and is merging my clique (MergeSync)
	 * 5. member (master or slave) of my clique fished and caught a better clique, is merging my clique (MergeSync)
	 * 6. member of my clique failed to hear sync and is assuming mastership (MasterSync)
	 * 7. member of my clique is sending work that includes synching info (WorkSync)
	 *
	 * Cannot assert sender is a master (msg.masterID could be different from senderID)
	 * Cannot assert self is slave
	 * Cannot assert msg.masterID equals clique.masterID
	 * Cannot assert self.isMaster => msg.masterID not equal clique.masterID
	 */

	/*
	 * Filter: returns true if message keeps my sync (from current or new master of my clique.)
	 *
	 * Side effect is call dropoutMonitor.heardSync() i.e. status of sync is good
	 *
	 * Each Sync has an offset:
	 * - zero or small (MasterSync)
	 * - or larger (MergeSync)
	 */
	static bool filterSyncMsg(SyncMessage* msg);


	static bool shouldTransmitSync();


	/*
	 * The essential effect of sync messages received in SyncSlot:
	 * - possible change clique
	 * - adjust schedule (might be small change)
	 * - adjust any merge in progress
	 *
	 * Sync messages received in other slots (FishSlot) have different effects.
	 */
	static void handleSyncMsg(SyncMessage* msg);


	/*
	 * Log sync message received in SyncSlot from inferior clique (often WorkSync, sometimes other.)
	 *
	 * Currently no substantive effect.
	 * But the comments are about sync behaviour.
	 */
	static void logWorseSync();
};
