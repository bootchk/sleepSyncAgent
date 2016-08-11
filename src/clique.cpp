
#include "clique.h"
#include "os.h"


// static
Schedule Clique::schedule;
MasterXmitSyncPolicy Clique::masterXmitSyncPolicy;
int Clique::masterID;


void Clique::reset(){
	masterID = myID();
	masterXmitSyncPolicy.reset();
}

bool Clique::isSelfMaster() {
	return masterID == myID();
}


void Clique::onMasterDropout() {
	// Failed to hear sync from master

	// Brute force: assume mastership.
	// Many may do this and suffer contention.
	reset();

	// TODO: alternative is a history of masters

}
