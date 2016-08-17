
#include <cassert>
#include "../os.h"

#include "clique.h"



// static
Schedule Clique::schedule;
MasterXmitSyncPolicy Clique::masterXmitSyncPolicy;
int Clique::masterID;



void Clique::reset(){
	masterID = myID();
	masterXmitSyncPolicy.reset();
	schedule.start();
	// assert clock is running and first period started but no tasks scheduled
}

bool Clique::isSelfMaster() {
	return masterID == myID();
}

bool Clique::isOtherCliqueBetter(int otherMasterID){
	return masterID < otherMasterID;
}

void Clique::onMasterDropout() {
	// Failed to hear sync from master

	// Brute force: assume mastership.
	// Many may do this and suffer contention.
	reset();

	// TODO: alternative is a history of masters
}

void Clique::initFromMsg(Message msg){
	assert(msg.type == Sync);
	masterID = msg.masterID;
	// TODO get offset
}


