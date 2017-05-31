
#include <cassert>
#include "../../config.h"
#include "clique.h"

#include "../globals.h"  // fishPolicy
#include "../policy/dropoutMonitor.h"
//#include "../policy/masterXmitSyncPolicy.h"
#include "../policy/adaptiveXmitSyncPolicy.h"

namespace {

// attributes of clique
SystemID masterID;

// collaborators
DropoutMonitor dropoutMonitor;

// Choices here:  Adaptive or Master
//static MasterXmitSyncPolicy masterXmitSyncPolicy;
AdaptiveXmitSyncPolicy masterXmitSyncPolicy;

} // namespace


// Schedule is public
Schedule Clique::schedule;


/*
 * Init is called only after a hw, power on reset POR
 */
void Clique::init(LongClockTimer* aLongClockTimer){
	log("Clique init\n");

	setSelfMastership();
	dropoutMonitor.reset();
	masterXmitSyncPolicy.reset();

	// Schedule uses LongClockTimer
	schedule.init(aLongClockTimer);

	// assert clock is running and first period started but no tasks scheduled
	// assert dropoutMonitor is initialized
	// assert xmitPolicy is initialized
	assert(isSelfMaster());
}




SystemID Clique::getMasterID() { return masterID; }

void Clique::setSelfMastership() {
	log("set self mastership\n");
	masterID = myID();
}

/*
 * In this design MasterSync from master and WorkSync msg from slave
 * both carry the MasterID of the clique master.
 * We might be the master, and so otherID (from WorkSync slave could already be the clique master.)
 *
 * In an alternative design (Work is separate and not piggybacked on Sync)
 * then you can make the assertion:
 * assert(otherID != myID());	// we can't hear our own sync
 */
void Clique::setOtherMastership(SystemID otherID) {
	log("set other master\n");
	masterID = otherID;
}

bool Clique::isSelfMaster() { return masterID == myID(); }


/*
 * Only master xmits FROM its sync slot.
 * And then with policy of randomness for collision avoidance.
 */
bool Clique::shouldXmitSync() {
	return isSelfMaster() && masterXmitSyncPolicy.shouldXmitSync();
}



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
 * All units use same comparison.  The direction is arbitrary.
 * For testing, it may help to swap it.
 * No need for equality, no unit can hear itself.
 * my > other means: clique/unit with least numerical ID is better clique
 */
bool Clique::isOtherCliqueBetter(SystemID otherMasterID){

#ifdef LEAST_ID_IS_BETTER_CLIQUE
	return masterID > otherMasterID;
#else
	return masterID < otherMasterID;
#endif

}




void Clique::heardSync() {
	// relevant to role Slave
	dropoutMonitor.heardSync();

	/*
	 * Relevant to role Master.
	 * Master can hear a WorkSync from Slaves.
	 * Assume all other clique members heard it too.
     * Minor optimization: Master that heard WorkSync from Slave does not need to send sync again soon.
	 * Avoids contention.
	 */
	if (isSelfMaster())
		masterXmitSyncPolicy.disarmForOneCycle();
}



void Clique::checkMasterDroppedOut() {
	if (dropoutMonitor.isDropout()) {
		// assert dropoutMonitor is reset
		log("    MASTER DROP OUT\n");
		onMasterDropout();
	}
}

void Clique::onMasterDropout() {
	/*
	 * Self unit has not heard sync from any member for a long time.
	 * Brute force: assume mastership.
	 * Other Slaves might do this and engender contention (many Masters.)
	 */
	// FUTURE: history of masters, self assume mastership only if was most recent master, thus avoiding contention.

	setSelfMastership();	// !!! changes role: self will start xmitting sync
	masterXmitSyncPolicy.reset();

	/*
	 * !!! Schedule is NOT changed. We may be able to recover by fishing nearby.
	 */

	/*
	 * Change fishing policy.
	 * This might help recover a Master who didn't permanently drop out:
	 * - busy or insufficient power temporarily
	 * - drifted too much
	 */
	fishPolicy.reset();
}




/*
 * An update, not necessarily a change.  Not assert result data different from current data.
 * The MasterID may be the same as current.
 * The offset may be zero.
 */
void Clique::updateBySyncMessage(SyncMessage* msg) {
	// assert (in Sync or Fish slot)
	assert(msg->carriesSync());

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



#ifdef NOT_USED
void Clique::initFromSyncMsg(SyncMessage* msg){
	assert(msg->type == Sync);	// require
	assert(msg->masterID != myID());	// invariant: we can't hear our own sync
	masterID = msg->masterID;
}
#endif
