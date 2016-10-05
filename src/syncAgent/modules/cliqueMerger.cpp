
#include <cassert>
#include "../../augment/random.h"
#include "cliqueMerger.h"


// Singleton data members
bool CliqueMerger::isActive = false;
DeltaTime CliqueMerger::offsetToMergee;
SystemID CliqueMerger::masterID;
Clique* CliqueMerger::owningClique;

// FUTURE: multiple notifies
//int CliqueMerger::notifyCountdown;



void CliqueMerger::initFromMsg(SyncMessage* msg){
	/*
	 * assert msg is sync heard in a fishing slot.
	 * Responsibility: know design and possibly adjust my schedule
	 * Save design so later, at endSyncSlot, we can schedule any mergeSlot.
	 */

	if (owningClique->isOtherCliqueBetter(msg->masterID))
		mergeMyClique(msg);
	else
		mergeOtherClique();
	// assert my schedule might have been adjusted
}



void CliqueMerger::mergeMyClique(SyncMessage* msg){
	/*
	 * Arrange state to start sending sync to my clique telling members to merge to other.
	 *
	 * Adjust self schedule.
	 *
	 * This instant becomes startSyncSlot in my adjusted schedule.
	 * My old syncSlot becomes a mergeSlot in my adjusted schedule.
	 *
	 * |S |W |  |  |F |  |  |S |  current schedule
	 *               ^------^   delta
	 *               |S |W ||M | |  |  |  |S |  my adjusted schedule
	 * Note adjusted schedule is not slot aligned with old
	 * and mergeSlot is not aligned with slots in adjusted schedule.
	 */
	// calculate delta from current schedule
	offsetToMergee = owningClique->schedule.deltaNowToNextSyncPoint();

	// after using current schedule above, can adjust to new schedule
	owningClique->schedule.adjustBySyncMsg(msg);

	masterID = msg->masterID;
}


void CliqueMerger::mergeOtherClique(){
	/*
	 * Start sending sync to other clique members telling them to merge to self's clique,
	 * and pass them offset from now to next SyncPoint
	 * Self (owning) schedule is unchanged.
	 * Now time becomes a mergeSlot in self schedule.
	 *
	 * * |S |W |  |  |F |  |  |S |  current schedule
	 *   ^-------------^            delta
	 *   |S |W |  |  | |M ||  |S |
	 *   mergeSlot is not aligned with slots in schedule.
	 *
	 * We don't care which SyncMessage we fished or which MasterID owned the other clique.
	 */

	offsetToMergee = owningClique->schedule.deltaNowToNextSyncPoint();
	// Self fished and caught other clique, and I will send MergeSync (contending with current other master)
	// but saying the new master is clique.masterID, not myID
	masterID = owningClique->masterID;
}


void CliqueMerger::adjustBySyncMsg(SyncMessage* msg) {
	/*
	 * This unit isMerger but heard a sync message in syncSlot that adjusts schedule.
	 * Make similar adjustment to this CliqueMerger, so that any MergeSync sent is at the correct time.
	 *
	 * My sync slot is moving later, merge time must move earlier in schedule to stay at same wall time.
	 */
	// TODO FUTURE fix
	// For now, do nothing, and xmit MergeSyncs at wrong time

	// offsetToMergee -= msg->deltaToNextSyncPoint;
	// TODO this is not right, result could be negative.  Need modulo.
	// Also, if the mergeSlot now overlaps sync or work slot?
}



bool CliqueMerger::shouldScheduleMerge() {
	/*
	 * Collision avoidance: choose randomly whether to schedule mergeSlot.
	 *
	 * Contention:
	 * - other fishers who caught the same merged clique
	 * - the master of the merged clique
	 *
	 * This implementation may remain in role isMerger a long time (a long random sequence of coin flips yielding heads.)
	 * The intent is to minimize scheduled tasks: we don't schedule mergeSlot until the start of period it occurs in.
	 *
	 * Alternative: choose once, a random time in the future, and schedule a mergeSlot in some distant sync period.
	 * But then the task is scheduled for a long time and many sync periods may happen meanwhile.
	 */
	assert(isActive);	// require
	return randBool();	// Fair coin flip
}



/* FUTURE send many mergeSyncs
 *
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


SyncMessage& CliqueMerger::makeMergeSync(SyncMessage& msg){
	msg.makeMergeSync(offsetToMergee, masterID);
	return msg;	// Returns msg passed.
}

