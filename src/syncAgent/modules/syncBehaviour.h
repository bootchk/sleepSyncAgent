
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
	 * Returns true if message keeps my sync (from current or new master of my clique.)
	 *
	 * Side effect is call dropoutMonitor.heardSync() i.e. status of sync is good
	 *
	 * Each Sync has an offset, could be zero or small (MasterSync) or larger (MergeSync)
	 */
	static bool doSyncMsg(SyncMessage* msg){
		// assert sync not from self (xmitter and receiver are exclusive)
		// assert self.isMaster || self.isSlave i.e. this code doesn't require any particular role
		assert (msg->carriesSync(msg->type));

		// Carries sync, but doesn't keep sync if not from better master
		bool doesMsgKeepSynch;

		// Most likely case first
		if (clique.isMsgFromMyClique(msg->masterID)) {
			/*
			 * Cases:
			 * - Master is MasterSync ing Slave self
			 * - Master or Slave fished another better clique and is MergeSync ing self
			 * - Slave is WorkSync ing Master or Slave self
			 */
			log("Sync from my clique (master or slave)\n");
			// WAS clique.changeBySyncMessage(msg);
			handleSyncMsg(msg);
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
			 * In a sync slot and heard from member of other worse clique:
			 * - MasterSync from Master
			 * - OR WorkSync from Master or Slave
			 * - OR MergeSync from Master or Slave (improper, my clique is better)
			 *
			 * Master of my clique (could be self) should continue as Master.
			 * Don't tell other clique: since their sync slot overlaps with mine,
			 * they should eventually hear my clique master's sync and relinquish mastership.
			 */
			// If it is WorkSync, we acted on the work but not the sync???
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
	 * - adjust schedule (might be small change)
	 * - adjust any merge in progress
	 *
	 * Sync messages received in other slots (FishSlot) have different effects.
	 */
	static void handleSyncMsg(SyncMessage* msg) {
		// assert current slot is Sync
		/*
		 * assert( clique.isMsgFromMyClique(msg->masterID) || !clique.isMsgFromMyClique(msg->masterID) );
		 * i.e. this does not assume the sync is from my current clique, or not from my current clique.
		 * If it is from my current clique, my Master does not change, but schedule is adjusted (usually small.)
		 * If it is from another clique, my Master does change, and schedule is adjusted.
		 */

		clique.updateBySyncMessage(msg);
		// assert endOfSyncPeriod changed or not changed

		if (role.isMerger()) {
			// Already merging an other clique, now merge other clique to updated sync slot time
			syncAgent.cliqueMerger.adjustMergerBySyncMsg(msg);
		}

		/*
		 * Minor optimization: Master that heard WorkSync from Slave does not need to send sync again soon.
		 * Avoids contention.
		 */
		if (clique.isSelfMaster()) clique.masterXmitSyncPolicy.disarmForOneCycle();
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
