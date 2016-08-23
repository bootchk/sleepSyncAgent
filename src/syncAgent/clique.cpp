
#include <cassert>

#include "../types.h"
#include "../radioWrapper.h"
#include "clique.h"


// static singleton
Schedule Clique::schedule;
MasterXmitSyncPolicy Clique::masterXmitSyncPolicy;
SystemID Clique::masterID;


void Clique::reset(){
	masterID = myID();
	masterXmitSyncPolicy.reset();
	schedule.startFreshAfterHWReset();
	// assert clock is running and first period started but no tasks scheduled
}

bool Clique::isSelfMaster() { return masterID == myID(); }
bool Clique::isOtherCliqueBetter(SystemID otherMasterID){ return masterID < otherMasterID; }

void Clique::onMasterDropout() {
	// Brute force: assume mastership.
	// Many units may do this and suffer contention.
	reset();

	// FUTURE: history of masters
}

void Clique::initFromSyncMsg(SyncMessage* msg){
	assert(msg->type == Sync);	// require
	assert(msg->masterID != myID());	// invariant: we can't hear our own sync
	masterID = msg->masterID;
}


