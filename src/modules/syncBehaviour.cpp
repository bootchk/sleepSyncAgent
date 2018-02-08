#include <cassert>

#include "syncBehaviour.h"

#include "../globals.h"  // clique

#include "../message/messageFactory.h"	// assertions

#include "../syncAgent/state/role.h"
#include "../syncAgent/syncAgent.h"
#include "../logging/logger.h"


namespace {

/*
 * Received sync while already merging.
 *
 * Cases:
 * - from my master, small adjustment
 * - from another clique, large adjustment
 *
 * Ideally, they are both handled by adjusting the ongoing merger.
 */
void handleSuperiorOrSameSyncWhileMerging(SyncMessage* msg) {
	(void) msg;

	if (clique.isMsgFromMyClique(msg->masterID)) {
		/*
		 * Allow current merger to continue: we want to notify other clique members we departed.
		 * Merger may drift since we aren't adjusting the merger.
		 */
	}
	else {
		// FUTURE: syncAgent.cliqueMerger.adjustMergerBySyncMsg(msg);
		// NOW: just abandon merging
		SyncAgent::stopMerger();
	}

}

}



bool SyncBehaviour::filterSyncMsg(SyncMessage* msg){

	assert (MessageFactory::doesCarrySync(msg->type));

	// assert sync not from self (xmitter and receiver are exclusive)
	// assert self.isMaster || self.isSlave i.e. this code doesn't require any particular role

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
		//log("Sync from my clique (master or slave)\n");
		// WAS clique.changeBySyncMessage(msg);
		handleSuperiorOrSameSyncMsg(msg);
		clique.heardSync();
		doesMsgKeepSynch = true;
	}
	else if (clique.isOtherCliqueBetter(msg->masterID)) {
		// Strictly better
		//log("Better master\n");
		handleSuperiorOrSameSyncMsg(msg);
		clique.heardSync();
		// assert(! isSelfMaster());
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



bool SyncBehaviour::shouldTransmitSync() {
	// Clique knows
	return clique.shouldTransmitSync();
}



void SyncBehaviour::handleSuperiorOrSameSyncMsg(SyncMessage* msg) {
	// assert current slot is Sync
	// Assert the other clique is superior or my clique (msg from the master, or another slave in my clique.)
	/*
	 * assert( clique.isMsgFromMyClique(msg->masterID) || !clique.isMsgFromMyClique(msg->masterID) );
	 * i.e. this does not assume the sync is from my current clique, or not from my current clique.
	 * If it is from my current clique, my Master does not change, but schedule is adjusted (usually small.)
	 * If it is from another clique, my Master does change, and schedule is adjusted.
	 */
	clique.updateBySyncMessage(msg);
	// assert endOfSyncPeriod changed or not changed

	if (MergerFisherRole::isMerger()) {
		handleSuperiorOrSameSyncWhileMerging(msg);
	}
}




void SyncBehaviour::logWorseSync() {
	// FUTURE: for now this is just logging, in future will record history
	if (clique.isSelfMaster()) {
		/*
		 * Sender has not heard my sync.
		 * Since I am still alive, they should not be assuming mastership.
		 * Could be assymetric communication (I can hear they, they cannot hear me.)
		 */
		Logger::logInferiorCliqueSyncSlotOfMaster();
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
		Logger::logInferiorCliqueSyncSlotOfSlave();
	}
}
