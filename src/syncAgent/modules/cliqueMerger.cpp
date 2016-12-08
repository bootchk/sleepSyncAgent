
#include <cassert>
#include "cliqueMerger.h"


namespace {

/*
 * Private state, redundant to outside logic about role.
 * Invariant: true => role is Merger
 * outside logic calls deactivate
 */
bool isActive = false;

// 2-way relation: Clique owns CliqueMerger, CliqueMerger uses owning Clique
Clique* owningClique;

MergeOffset offsetToMergee;
SystemID masterID;


/*
 * Many of the following routines are coded nievely, to be understandable and correct.
 *
 * They could be optimized: substitute and algebraically simplify yielding a simpler formula.
 */

/*
 * Allow for future changes and more robustness.
 * For now, assert little time has passed executing code since message arrived, so nowTime() is close enough.
 * FUTURE: record TOA when message arrives.
 */
LongTime messageTimeOfArrival() {
	return owningClique->schedule.nowTime();
}

LongTime middleOfNextSyncSlotOfFisher() {
	LongTime result = owningClique->schedule.timeOfNextSyncPoint() + ScheduleParameters::SlotDuration;
	return result;
}

LongTime nextSyncPointOfFisher() {
	LongTime result = owningClique->schedule.timeOfNextSyncPoint();
	return result;
}
/*
 * Assert now we just received a SyncMsg from middle of SyncSlot of Catch clique
 */
// Time in future when Catch will be in middle of next SyncSlot
LongTime middleOfNextSyncSlotOfCatch() {
	LongTime result = messageTimeOfArrival() + ScheduleParameters::NormalSyncPeriodDuration;
	return result;
}
// Time in past when Catch started SyncSlot
LongTime pastSyncPointOfCatch() {
	LongTime result = messageTimeOfArrival() - owningClique->schedule.halfSlotDuration();
	return result;
}



DeltaTime offsetForMergeMyClique() {
	/*
	TODO this might be an optimized equivalent.
	owningClique->schedule.deltaNowToNextSyncPoint()
				+ ScheduleParameters::SlotDuration;	// plus two half slots
	*/
	DeltaTime result = LongClock::clampedTimeDifference(middleOfNextSyncSlotOfFisher(), pastSyncPointOfCatch())
			% ScheduleParameters::NormalSyncPeriodDuration;
	return result;
}

DeltaTime offsetForMergeOtherClique() {
	/*
	 * TODO this might be an optimized equivalent.
	 * owningClique->schedule.deltaPastSyncPointToNow();
	 */
	DeltaTime result =  LongClock::clampedTimeDifference(middleOfNextSyncSlotOfCatch(), nextSyncPointOfFisher())
		% ScheduleParameters::NormalSyncPeriodDuration;

	return result;
}




/*
 * Two variants: mergeMy and mergeOther
 *
 * For both, only the offsetToMergee need be calculated.
 * A DeltaSync will be calculated at the time self sends MergeSync.
 *
 * For mergeMy, self's schedule is adjusted.  For mergeOther, not adjust self's schedule.
 */
void initMergeMyClique(SyncMessage* msg){
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
	 *                  ^H----------H^ offsetToMergee
	 * Note:
	 * - adjusted schedule is not slot aligned with old.
	 * - mergeSlot is not aligned with slots in adjusted schedule.
	 * - want total offset to include two halfSlotDurations
	 * -- one to get back to syncPoint of adjusted schedule
	 * -- ont to get forward to middle of syncSlot of old, unadjusted schedule
	 */
	log("Merge my clique\n");

	// Using unadjusted schedule
	offsetToMergee.set(offsetForMergeMyClique());

	// FUTURE migrate this outside and return result to indicate it should be done
	// After using current clique above, change my clique (new master and new schedule)
	owningClique->updateBySyncMessage(msg);

	// Merging my clique into clique ID of message
	masterID = msg->masterID;
	assert(!owningClique->isSelfMaster());	// Even if true previously
}


void initMergeOtherClique(){
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
	offsetToMergee.set(offsetForMergeOtherClique());

	// No adjustment to self schedule

	// Self fished and caught other clique, and self will send MergeSync (contending with current other master)
	// but saying master is self's clique.masterID, not necessarily self's masterID
	masterID = owningClique->getMasterID();
}

} // namespace


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



MergeOffset CliqueMerger::getOffsetToMergee() {
	return offsetToMergee;
}
