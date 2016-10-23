
#include <cassert>
#include "../../platform/platform.h"
#include "clique.h"


// static singleton
Schedule Clique::schedule;
MasterXmitSyncPolicy Clique::masterXmitSyncPolicy;
SystemID Clique::masterID;


void Clique::reset(){
	log("Clique reset\n");
	masterID = myID();
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
bool Clique::isMyMaster(SystemID otherMasterID){ return masterID == otherMasterID; }

// All units use same comparison.  The direction is arbitrary.  For testing, it may help to swap it.
// No need for equality, no unit can hear itself.
bool Clique::isOtherCliqueBetter(SystemID otherMasterID){ return masterID > otherMasterID; }

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



void Clique::changeBySyncMessage(SyncMessage* msg) {
	// assert in Sync or Fish slot
	assert(msg->type == MasterSync || msg->type == MergeSync);

	// Change master
	setOtherMastership(msg->masterID);
	assert(!isSelfMaster()); // even if I was before

	// FUTURE clique.historyOfMasters.update(msg);

	// Change schedule.
	// Regardless: from my master (small offset) or from another clique (large offset)
	schedule.adjustBySyncMsg(msg);
}
