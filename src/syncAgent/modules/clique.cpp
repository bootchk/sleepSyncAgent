
#include <cassert>
#include "../../platform/platform.h"
#include "clique.h"


// static singleton
Schedule Clique::schedule;
DropoutMonitor Clique::dropoutMonitor;
SystemID Clique::masterID;

// Choices here:  Adaptive or Master
AdaptiveXmitSyncPolicy Clique::masterXmitSyncPolicy;


void Clique::reset(){
	log("Clique reset\n");

	masterID = myID();	// This unit (with ID given by myID() ) is master of clique

	masterXmitSyncPolicy.reset();
	schedule.startFreshAfterHWReset();
	// assert clock is running and first period started but no tasks scheduled
}

void Clique::setSelfMastership() {
	log("Self mastership\n");
	masterID = myID();
}

void Clique::setOtherMastership(SystemID otherID) {
	assert(otherID != myID());	// we can't hear our own sync
	log("Other mastership\n");
	masterID = otherID;
}

bool Clique::isSelfMaster() { return masterID == myID(); }


/*
 * Is msg from member of my clique?
 *
 * Formerly, only the Master sent MasterSync.
 * Now, a Slave member of my clique may send a WorkSync, still identifying the Master i.e. clique
 *
 * (When a MergeSync, might be from a recent member of my clique, now a member of identified clique.)
 *
 */
bool Clique::isMsgFromMyClique(SystemID otherMasterID){ return masterID == otherMasterID; }

/*
 * All units use same comparison.  The direction is arbitrary.  For testing, it may help to swap it.
 * No need for equality, no unit can hear itself.
 * my > other means: clique/unit with least numerical ID is better clique
 */
bool Clique::isOtherCliqueBetter(SystemID otherMasterID){ return masterID > otherMasterID; }


void Clique::checkMasterDroppedOut() {
	if (dropoutMonitor.isDropout()) {
		// assert dropoutMonitor is reset
		log("Master dropped out\n");
		onMasterDropout();
	}
}

void Clique::onMasterDropout() {
	// Brute force: assume mastership.
	// Many units may do this and suffer contention.
	reset();

	// FUTURE: history of masters
}

#ifdef NOT_USED
void Clique::initFromSyncMsg(SyncMessage* msg){
	assert(msg->type == Sync);	// require
	assert(msg->masterID != myID());	// invariant: we can't hear our own sync
	masterID = msg->masterID;
}
#endif


/*
 * An update, not necessarily a change.  Not assert result data different from current data.
 * The MasterID may be the same as current.
 * The offset may be zero.
 */
void Clique::updateBySyncMessage(SyncMessage* msg) {
	// assert (in Sync or Fish slot)
	assert(msg->carriesSync(msg->type));

	/*
	 * !!! Update.  Not assert that msg.MasterID != self.masterID:
	 * a WorkSync from a Slave carries MasterID of clique which could match my MasterID when self is Master
	 */
	setOtherMastership(msg->masterID);
	// Not assert master changed

	/*
	 * Self has heard another unit, retard policy.
	 * (The sync message may have same MasterID as self's clique, but msg is definitely from another unit.)
	 * If self unit not master, won't be xmitting sync now,
	 * but if self unit ever assumes mastership,
	 * policy will then be in advanced stage.
	 */
	masterXmitSyncPolicy.advanceStage();

	// FUTURE clique.historyOfMasters.update(msg);

	// Change schedule.
	// Regardless: from my master (small offset) or from another clique (large offset)
	schedule.adjustBySyncMsg(msg);
}

