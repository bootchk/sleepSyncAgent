
#include <cassert>

#include "clique.h"

#include "../policy/fishPolicy.h"
#include "../policy/dropoutMonitor.h"

//#include "../policy/masterXmitSyncPolicy.h"
#include "../policy/syncPolicy/adaptiveXmitSyncPolicy.h"

#include "../message/messageFactory.h"

#include "../logging/logger.h"



namespace {

// collaborators
DropoutMonitor dropoutMonitor;

// Choices here:  Adaptive or Master
//static MasterTransmitSyncPolicy masterTransmitSyncPolicy;
AdaptiveTransmitSyncPolicy masterTransmitSyncPolicy;

} // namespace




// Schedule is public
Schedule Clique::schedule;


/*
 * Init is called only after a hw, power on reset POR
 */
void Clique::init(){
	Logger::log("Clique init\n");

	setSelfMastership();
	dropoutMonitor.reset();
	masterTransmitSyncPolicy.reset();

	schedule.init();

	// assert clock is running and first period started but no tasks scheduled
	// assert dropoutMonitor is initialized
	// assert xmitPolicy is initialized
	assert(isSelfMaster());
}



/*
 * Only master xmits FROM its sync slot.
 * And then with policy of randomness for collision avoidance.
 */
bool Clique::shouldTransmitSync() {
	return isSelfMaster() && masterTransmitSyncPolicy.shouldTransmitSync();
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
		masterTransmitSyncPolicy.disarmForOneCycle();
}



void Clique::checkMasterDroppedOut() {
	if (dropoutMonitor.isDropout()) {
		// assert dropoutMonitor is reset
		Logger::logMasterDropout();
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
	masterTransmitSyncPolicy.reset();

	/*
	 * !!! Schedule is NOT changed. We may be able to recover by fishing nearby.
	 */

	/*
	 * Change fishing policy.
	 * This might help recover a Master who didn't permanently drop out:
	 * - busy or insufficient power temporarily
	 * - drifted too much
	 */
	Logger::log("reset FishPolicy\n");
	SyncRecoveryFishPolicy::reset();
}




/*
 * Implementation:
 * Three steps:
 * 1) update mastership (!!! might change Master/Slave role)
 * 2) update policy
 * 3) update schedule
 * 4) FUTURE update history of masters
 *
 * An update, not necessarily a change.  Not assert result data different from current data.
 * The MasterID may be the same as current.
 * The offset may be zero.
 */
void Clique::updateBySyncMessage(SyncMessage* msg) {
	// assert (in Sync or Fish slot)
	assert (MessageFactory::carriesSync(msg->type));

	// 1) update master
	setOtherMastership(msg->masterID);
	/*
	 * Not assert master changed. Not assert that msg.MasterID != self.masterID:
	 * a WorkSync from a Slave carries MasterID of clique which could match my MasterID when self is Master
	 */

	/*
	 * 2) update policy
	 * See advanceStage() comments
	 */
	masterTransmitSyncPolicy.advanceStage();

	// FUTURE clique.historyOfMasters.update(msg);

	/*
	 *  3) update schedule
	 *  Regardless: from my master (small offset) or from another clique (large offset)
	 */
	schedule.adjustBySyncMsg(msg);
}



#ifdef NOT_USED
void Clique::initFromSyncMsg(SyncMessage* msg){
	assert(msg->type == Sync);	// require
	assert(msg->masterID != System::ID());	// invariant: we can't hear our own sync
	masterID = msg->masterID;
}
#endif
