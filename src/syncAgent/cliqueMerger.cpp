
#include <cassert>
#include "cliqueMerger.h"


// Singleton data members
bool CliqueMerger::active = false;
int CliqueMerger::offsetToMergee;
int CliqueMerger::masterID;
Clique* CliqueMerger::clique;	// cliqueMerger knows parent clique
//int CliqueMerger::notifyCountdown;


void CliqueMerger::initFromMsg(Message msg){
	/*
	 * assert msg is sync heard in a fishing slot.
	 * Set this state.
	 * At endSyncSlot, we schedule any mergeSlot.
	 */

	//if (clique.isOtherCliqueBetter(otherClique)){
			mergeMyClique(msg);
	//	}
	//	else{
			mergeOtherClique(msg);
	//	}
}



void CliqueMerger::mergeMyClique(Message msg){
	/*
	 * Arrange state to start sending sync to my clique telling members to merge to other.
	 *
	 * Adjust self schedule.
	 *
	 * This instant becomes startSyncSlot in my adjusted schedule.
	 * My old syncSlot becomes a mergeSlot in my adjusted schedule.
	 */
	clique->schedule.adjustBySyncMsg(msg);	// ????
	offsetToMergee = clique->schedule.deltaNowToStartNextSync();	//TODO
	masterID = msg.masterID;
}


void CliqueMerger::mergeOtherClique(Message msg){
	/*
	 * Start sending sync to other clique telling members to merge to self's clique
	 * Self keeps old schedule.
	 * This time becomes a mergeSlot in my new schedule.
	 */
	offsetToMergee = clique->schedule.deltaStartThisSyncToNow();
	// Self fished and caught other clique, and I will send MergeSync (contending with current other master)
	// but saying the new master is clique.masterID, not myID
	masterID = clique->masterID;
}

void CliqueMerger::adjustBySyncMsg(Message msg) {
	// TODO
	;
}

// TODO no setter, public flag
bool CliqueMerger::isActive() {
	return active;
}

bool CliqueMerger::shouldScheduleMerge() {
	/*
	 * Collision avoidance: choose randomly whether to schedule mergeSlot.
	 *
	 * This implementation may remain in role isMerger a long time (a long random sequence of heads.)
	 * The intent is to minimize scheduled tasks: we don't schedule mergeSlot until the start of period it occurs in.
	 *
	 * Alternative: choose once, a random time in the future, and schedule a mergeSlot in some distant sync period.
	 * But then the task is scheduled for a long time.
	 */
	assert(isActive());	// require
	// TODO CA, random chance
	return true;
}



/* FUTURE
 * // called after sending a merging syncLength
// xmit finite count of MergeSync
bool CliqueMerger::checkCompletionOfMergerRole() {
	assert(isActive());	// require
	notifyCountdown--;
	bool result = false;
	if ( notifyCountdown <= 0 ){
		result = true;
		active = false;
	}
	return result;
}
*/

/*
void CliqueMerger::activateWithOffsetAndMasterID(
		int offset,
		int MasterID){
// TODO
}
*/

int CliqueMerger::timeOfNextMergeWake(){
	// Knows time of mergeSlot i.e. what normally sleeping slot of current period
	// TODO calculate time
	return 1;
}


