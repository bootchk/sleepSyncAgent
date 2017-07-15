
#include <cassert>
#include "cliqueMerger.h"
#include "../../augment/timeMath.h"
#include "../scheduleParameters.h"
#include "../globals.h"

#include "../message/messageFactory.h"


namespace {

/*
 * Private state, redundant to outside logic about role.
 * Invariant: true => role is Merger
 * outside logic calls deactivate
 */
bool isActive = false;

// 2-way relation: Clique owns CliqueMerger, CliqueMerger uses owning Clique
Clique* owningClique;

/*
 * See definition of MergeOffset: a DeltaTime.
 */
MergeOffset offsetToMergeeSyncSlotMiddle;


SystemID superiorMasterID;
SystemID inferiorMasterID;


/*
 * Many of the following routines are coded nievely, to be understandable and correct.
 *
 * They could be optimized: substitute and algebraically simplify yielding a simpler formula.
 */

/*
 * Allow for future changes and more robustness.
 * For now, assert little time has passed executing code since message arrived, so nowTime() is close enough.
 * FUTURE: record TOA when message arrives.  See elsewhere, already recorded?
 */
LongTime messageTimeOfArrival() {
	// WAS return owningClique->schedule.nowTime();
	return LongClock::nowTime();
}

LongTime middleOfNextSyncSlotOfFisher() {
	LongTime result = owningClique->schedule.timeOfNextSyncPoint() + ScheduleParameters::DeltaToSyncSlotMiddle;
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
	LongTime result = messageTimeOfArrival() - ScheduleParameters::DeltaToSyncSlotMiddle;
	// TODO minus a halfMessageLatency or other adjustment???
	// WAS: owningClique->schedule.halfSlotDuration();
	return result;
}


/*
 * MergeOffset is TimeToXmitMergeSync - SyncPointOfSender
 */

/*
 * Here, self will be sender, but will be on schedule of Catch clique
 */
DeltaTime offsetForMergeMyClique() {
	/*
	XXX this might be an optimized equivalent.
	owningClique->schedule.deltaNowToNextSyncPoint()
				+ ScheduleParameters::SlotDuration;	// plus two half slots
	*/
	DeltaTime result = TimeMath::clampedTimeDifference(middleOfNextSyncSlotOfFisher(), pastSyncPointOfCatch())
			% ScheduleParameters::NormalSyncPeriodDuration;
	return result;
}

/*
 * Here, self will be sender, and on self's same schedule
 */
DeltaTime offsetForMergeOtherClique() {
	/*
	 * XXX this might be an optimized equivalent.
	 * owningClique->schedule.deltaPastSyncPointToNow();
	 */
	DeltaTime result =  TimeMath::clampedTimeDifference(middleOfNextSyncSlotOfCatch(), nextSyncPointOfFisher())
		% ScheduleParameters::NormalSyncPeriodDuration;

	return result;
}




/*
 * Two variants: mergeMy and mergeOther
 *
 * For both, only the offsetToMergeeSyncSlotMiddle need be calculated.
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
	 *                  ^H----------H^ offsetToMergeeSyncSlotMiddle
	 * Note:
	 * - adjusted schedule is not slot aligned with old.
	 * - mergeSlot is not aligned with slots in adjusted schedule.
	 * - want total offset to include two halfSlotDurations
	 * -- one to get back to syncPoint of adjusted schedule
	 * -- ont to get forward to middle of syncSlot of old, unadjusted schedule
	 */
	log("Merge my clique\n");

	// Using self unadjusted schedule
	offsetToMergeeSyncSlotMiddle.set(offsetForMergeMyClique());

	/*
	 * Merging my inferior clique into superior clique ID of fished message
	 */
	superiorMasterID = msg->masterID;
	inferiorMasterID = owningClique->getMasterID();


	// FUTURE migrate this outside and return result to indicate it should be done
	/*
	 * After using current clique above, change my clique (new master and new schedule)
	 */
	owningClique->updateBySyncMessage(msg);



	assert(!owningClique->isSelfMaster());	// Even if true previously
	/*
	 * Post conditions:
	 * Self is on a new clique and schedule (that of the fished message.)
	 * Self is not master.
	 * Self is Role Merger.
	 * Self will send MergeSync in SyncSlot of former clique.
	 */
}

/*
 * Pre conditions:
 * Self fished and caught other clique.
 * Other clique is inferior.
 */
void initMergeOtherClique(SyncMessage* msg){
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
	offsetToMergeeSyncSlotMiddle.set(offsetForMergeOtherClique());

	// No adjustment to self schedule

	/*
	 * Self is Master or Slave.
	 * master of superior clique is self's clique.masterID, not necessarily self's ID
	 */
	superiorMasterID = owningClique->getMasterID();
	inferiorMasterID = msg->masterID;

	/*
	 * Post conditions
	 * Self in Merge Role.
	 * Will send MergeSync (contending with master of other inferior clique.)
	 * Self's clique and schedule unchanged.
	 */
}

} // namespace

void CliqueMerger::deactivate(){ isActive = false; }

void CliqueMerger::initFromMsg(SyncMessage* msg){
	/*
	 * msg heard in a fishing slot.
	 * Responsibility: know design and possibly adjust my schedule
	 * Save design so later, at endSyncSlot, we can schedule any mergeSlot.
	 */

	assert (MessageFactory::carriesSync(msg->type));
	log("Other Master ID: \n");
	logLongLong(msg->masterID);

	if (owningClique->isOtherCliqueBetter(msg->masterID))
		initMergeMyClique(msg);
	else
		initMergeOtherClique(msg);

	isActive = true;
	assert(isActive);
	// assert my schedule might have been adjusted
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
	log("Not implemented: adjustMerger\n");
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

	// offsetToMergeeSyncSlotMiddle -= msg->deltaToNextSyncPoint;
	// FUTURE this is not right, result could be negative.  Need modulo.
	// Also, if the mergeSlot now overlaps sync or work slot?
#endif
	return;
}




SyncMessage* CliqueMerger::makeMergeSync(){
	// side effect on serializer's message templates
	assert(isActive);

	/*
	 * masterID from this CliqueMerger
	 *
	 * DeltaSync calculate now.
	 * The call here must be just before sending.
	 */
	DeltaTime rawOffset = owningClique->schedule.deltaNowToNextSyncPoint();

	return MessageFactory::initMergeSyncMessage(rawOffset, superiorMasterID, inferiorMasterID);
}



// Return pointer to internal data structure, that is constant to the caller
const MergeOffset* CliqueMerger::getOffsetToMergeeSyncSlotMiddle() {
	return &offsetToMergeeSyncSlotMiddle;
}
