
/*
 * Knows essential behaviours of sync algorithm.
 *
 * Collaborates (called by) SyncSlot and other variants of SyncSlot.
 * But does not depend on Slot.
 *
 * History: formerly part of SyncSlot.
 */

#include "../logMessage.h"


class SyncBehaviour {
public:

	/*
	 * Cases for sync messages:
	 * 1. my cliques current master (usual)
	 * 2. other clique master happened to start schedule coincident with my schedule
	 * 3. other clique master clock drifts so schedules coincide
	 * 4. member (master or slave) of other, better clique fished, caught my clique and is merging my clique
	 * 5. a member of my clique failed to hear sync and is assuming mastership
	 * 6. a member of my clique is sending work that includes synching info
	 *
	 * Cannot assert sender is a master (msg.masterID could be different from senderID)
	 * Cannot assert self is slave
	 * Cannot assert msg.masterID equals clique.masterID
	 * Cannot assert self.isMaster => msg.masterID not equal clique.masterID
	 */

	/*
	 * Returns true if sync message keeps my sync (from current or new master of my clique.)
	 *
	 * Each Sync has an offset, could be zero or small (MasterSync) or larger (MergeSync)
	 */
	static bool doSyncMsg(SyncMessage* msg){
		// assert SyncMsg is subtype MasterSync OR MergeSync
		// assert sync not from self (xmitter and receiver are exclusive)
		// assert self.isMaster || self.isSlave i.e. this code doesn't require any particular role

		bool doesMsgKeepSynch;

		// Most likely case first
		if (clique.isMyMaster(msg->masterID)) {
			/*
			 * My Master could have fished another better clique and be MergeSyncing self
			 *
			 * A slave member of my clique is sending synch (unusual, since self could be master.)
			 */
			log("Sync from my clique (master or slave)\n");
			clique.changeBySyncMessage(msg);
			clique.dropoutMonitor.heardSync();
			doesMsgKeepSynch = true;
		}
		else if (clique.isOtherCliqueBetter(msg->masterID)) {
			// Strictly better
			log("Better master\n");
			handleSyncMsg(msg);
			clique.dropoutMonitor.heardSync();
			doesMsgKeepSynch = true;
		}
		else {
			/*
			 * Heard MasterSync in SyncSlot from other Master of other worse clique.
			 * OR heard MergeSync from Master or Slave of other worse clique.
			 * Master of my clique (could be self) should continue as Master.
			 * Don't tell other clique: since their sync slot overlaps with mine,
			 * they should eventually hear my clique master's sync and relinquish mastership.
			 */
			logWorseSync();
			// !!! SyncMessage does not keep me in sync: not dropoutMonitor.heardSync();
			doesMsgKeepSynch = false;
		}
		return doesMsgKeepSynch;
	}



	/*
	 * Deciding whether to xmit MasterSync.
	 * Only master xmits FROM its sync slot.
	 * And then with policy: a coin-flip, for collision avoidance.
	 */
	static bool shouldTransmitSync() {
		return clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync();
	}


	/*
	 * The essential effect of sync messages received in SyncSlot:
	 * - possible change clique
	 * - adjust schedule
	 * - adjust any merge in progress
	 *
	 * Sync messages received in other slots (FishSlot) have different effects.
	 */
	static void handleSyncMsg(SyncMessage* msg) {
		// assert current slot is Sync
		assert(msg->masterID != clique.getMasterID());

		clique.changeBySyncMessage(msg);
		// assert endOfSyncPeriod was changed

		if (syncAgent.role.isMerger()) {
			// Already merging an other clique, now merge other clique to updated sync slot time
			syncAgent.cliqueMerger.adjustMergerBySyncMsg(msg);
		}
	}



	// DEBUG only, no substantive effect
	static void logWorseSync() {
		// FUTURE: for now this is just logging, in future will record history
		if (clique.isSelfMaster()) {
			/*
			 * Sender has not heard my sync.
			 * Since I am still alive, they should not be assuming mastership.
			 * Could be assymetric communication (I can hear they, they cannot hear me.)
			 */
			log("Worse sync while self is master.\n");
		}
		else { // self is slave
			/*
			 * Sender has not heard my master's sync.
			 * My master may have dropped out (and I just don't know yet), and they are assuming mastership.
			 * Wait until I discover my master dropout.
			 */
			// FUTURE: if msg.masterID < myID(), I should assume mastership instead of sender
			// FUTURE: if msg.masterID > myID() record msg.masterID in my historyOfMasters
			// so when I discover dropout, I will defer to msg.masterID
			log("Worse sync while self is slave.\n");
		}
	}
};
