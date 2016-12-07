
#include <cassert>
#include "cliqueMerger.h"


// Singleton data members
bool CliqueMerger::isActive = false;
MergeOffset CliqueMerger::offsetToMergee;
SystemID CliqueMerger::masterID;
Clique* CliqueMerger::owningClique;



void CliqueMerger::initFromMsg(SyncMessage* msg){
	/*
	 * msg heard in a fishing slot.
	 * Responsibility: know design and possibly adjust my schedule
	 * Save design so later, at endSyncSlot, we can schedule any mergeSlot.
	 */

	assert(msg->carriesSync(msg->type));
	log("Other Master ID: \n");
	logLongLong(msg->masterID);

	if (owningClique->isOtherCliqueBetter(msg->masterID))
		initMergeMyClique(msg);
	else
		initMergeOtherClique();

	isActive = true;
	assert(isActive);
	// assert my schedule might have been adjusted
}

void CliqueMerger::deactivate(){ isActive = false; }


void CliqueMerger::initMergeMyClique(SyncMessage* msg){
	/*
	 * Arrange state to start sending sync to my clique telling members to merge to other.
	 *
	 * Adjust self schedule.
	 *
	 * This instant becomes startSyncSlot in my adjusted schedule.
	 * My old syncSlot becomes a mergeSlot in my adjusted schedule.
	 *
	 * |S..|W..|...|...|F..|...|...|S..|  current schedule
	 *                    ^--------^   deltaNowToNextSyncPoint
	 *                  |S..|W..|....M........|S..|  my adjusted schedule
	 *                  ^H----------H^
	 * Note:
	 * - adjusted schedule is not slot aligned with old.
	 * - mergeSlot is not aligned with slots in adjusted schedule.
	 * - want total offset to include two halfSlotDurations
	 * -- one to get back to syncPoint of adjusted schedule
	 * -- ont to get forward to middle of syncSlot of old, unadjusted schedule
	 */
	log("Merge my clique\n");

	// calculate delta from current, unadjusted schedule
	setOffsetToMergee(
			owningClique->schedule.deltaNowToNextSyncPoint()
			+ ScheduleParameters::SlotDuration);	// plus two half slots

	// FUTURE migrate this outside and return result to indicate it should be done
	// After using current clique above, change my clique (new master and new schedule)
	owningClique->updateBySyncMessage(msg);

	masterID = msg->masterID;
	assert(!owningClique->isSelfMaster());	// Even if true previously
}


void CliqueMerger::initMergeOtherClique(){
	/*
	 * Start sending sync to other clique members telling them to merge to self's clique,
	 * and pass them offset from now to next SyncPoint
	 * Self (owning) schedule is unchanged.
	 * Now time becomes a mergeSlot in self schedule.
	 *
	 * * |S..|W..|...|...|F..|...|...|S |  current schedule
	 *   ^------------------^            delta
	 *   |S..|W..|..........M........|S |
	 *   mergeSlot is not aligned with slots in schedule.
	 *
	 * We don't care which SyncMessage we fished or which MasterID owned the other clique.
	 */
	log("Merge other clique\n");

	// WRONG setOffsetToMergee(owningClique->schedule.deltaNowToNextSyncPoint());
	setOffsetToMergee(owningClique->schedule.deltaPastSyncPointToNow());

	// Self fished and caught other clique, and I will send MergeSync (contending with current other master)
	// but saying the new master is my clique.masterID, not necessarily myID
	masterID = owningClique->getMasterID();
}


void CliqueMerger::adjustMergerBySyncMsg(SyncMessage* msg) {
	/*
	 * This unit's role isMerger (cliqueMerger.isActive)
	 * Heard a sync message in syncSlot that adjusts this units schedule.
	 * Make similar adjustment to this CliqueMerger, so that any MergeSync sent is at the correct time.
	 *
	 * My sync slot is moving, merge time must move to stay at same wall time (to hit the sync slot of mergee.)
	 */
	// assert current slot is Sync
	// assert msg is MasterSync (small offset) or MergeSync (large offset)
	// assert endOfSyncPeriod was changed, it now is my new end of sync period

	(void) msg;	// temporarily suppress warnings
	log("NI adjustMerger\n");
	// TODO implement adjustMerger
	/*
	 * FUTURE, when more than two units.
	 * When only two units, it doesn't matter if MergeSync is off, since there is no third unit to hear it.
	 * Unless it interferes with other slot?
	 */
	assert(isActive);

#ifdef FUTURE
	// The new next sync point is not at the old one before this msg adjusted endSyncPoint.

	DeltaTime deltaStartSyncPeriodToNewNextSyncPoint = owningClique->schedule.halfSlotDuration() + msg->deltaToNextSyncPoint;
	(void) deltaStartSyncPeriodToNewNextSyncPoint;
	// For now, do nothing, and xmit MergeSyncs at wrong time

	// offsetToMergee -= msg->deltaToNextSyncPoint;
	// FUTURE this is not right, result could be negative.  Need modulo.
	// Also, if the mergeSlot now overlaps sync or work slot?
#endif
	return;
}




void CliqueMerger::makeMergeSync(SyncMessage& msg){
	// side effect on passed msg
	assert(isActive);

	/*
	 * masterID from this CliqueMerger
	 *
	 * DeltaSync calculate now.
	 * The call here must be just before sending.
	 */
	DeltaSync deltaToNextSyncPoint = owningClique->schedule.deltaNowToNextSyncPoint();

	msg.makeMergeSync(deltaToNextSyncPoint, masterID);
}


void CliqueMerger::setOffsetToMergee(DeltaTime offset) {
	(void) SEGGER_RTT_printf(0, "Merger offset: %u\n", offset);
	// May throw assertion
	offsetToMergee.set(offset);
}

MergeOffset CliqueMerger::getOffsetToMergee() {
	return offsetToMergee;
}
