
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

PeriodTime periodTimeToMergeSlotStart;


SystemID superiorMasterID;
SystemID inferiorMasterID;


/*
 * Many of the following routines are coded nievely, to be understandable and correct.
 *
 * They could be optimized: substitute and algebraically simplify yielding a simpler formula.
 */



LongTime middleOfNextSyncSlotOfFisher() {
	LongTime result = owningClique->schedule.timeOfNextSyncPoint() + ScheduleParameters::DeltaSyncPointToSyncSlotMiddle;
	return result;
}

LongTime nextSyncPointOfFisher() {
	LongTime result = owningClique->schedule.timeOfNextSyncPoint();
	return result;
}


/*
 * Assert we just received a SyncMsg from middle of SyncSlot of Catch clique.
 *
 * These return LongTime on local LongClock of events on a caught other unit.
 */
// Time in future when Catch will be in middle of next SyncSlot
LongTime middleOfNextSyncSlotOfCatch() {
	LongTime result = SyncMessage::timeOfTransmittal()
			+ ScheduleParameters::NormalSyncPeriodDuration;
	return result;
}

// Time in past of Catch's SyncPoint
LongTime pastSyncPointOfCatch() {
	LongTime result = SyncMessage::timeOfTransmittal()
			- ScheduleParameters::DeltaSyncPointToSyncSlotMiddle;
	// WAS: owningClique->schedule.halfSlotDuration();
	return result;
}


/*
 * PeriodTime to transmit MergeSync is TimeToXmitMergeSync - SyncPointOfSender
 *
 * Calculated once, read each time we send MergeSync,
 * (An alternative design is to recalculate it if our Schedule is adjusted: not implemented.)
 *
 * Calculated in two steps:
 * - raw
 * - plus adjustments
 */

DeltaTime adjustedRawTimeToMergeClique(DeltaTime rawTime) {
	// Adjust time to transmit to get the time we should start MergeSlot
	return rawTime - ScheduleParameters::PreflightDelta;
}

/*
 * Self will be sender to inferior former clique, self will be on schedule of Catch's superior clique
 */
DeltaTime periodTimeToMergeMyClique() {
	/*
	XXX this might be an optimized equivalent.
	owningClique->schedule.deltaNowToNextSyncPoint()
				+ ScheduleParameters::SlotDuration;	// plus two half slots
	*/
	DeltaTime rawTime = TimeMath::clampedTimeDifference(middleOfNextSyncSlotOfFisher(), pastSyncPointOfCatch())
			% ScheduleParameters::NormalSyncPeriodDuration;
	return adjustedRawTimeToMergeClique(rawTime);
}

/*
 * Self will be sender, on self's existing schedule
 */
DeltaTime periodTimeToMergeOtherClique() {
	/*
	 * XXX this might be an optimized equivalent.
	 * owningClique->schedule.deltaPastSyncPointToNow();
	 */
	DeltaTime rawTime =  TimeMath::clampedTimeDifference(middleOfNextSyncSlotOfCatch(), nextSyncPointOfFisher())
		% ScheduleParameters::NormalSyncPeriodDuration;

	return adjustedRawTimeToMergeClique(rawTime);
}




/*
 * Two variants: mergeMy and mergeOther
 *
 * For both, only the periodTimeToMergeeSyncSlotMiddle need be calculated.
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
	 *                  ^H----------H^ periodTimeToMergeeSyncSlotMiddle
	 * Note:
	 * - adjusted schedule is not slot aligned with old.
	 * - mergeSlot is not aligned with slots in adjusted schedule.
	 * - want total periodTime to include two halfSlotDurations
	 * -- one to get back to syncPoint of adjusted schedule
	 * -- ont to get forward to middle of syncSlot of old, unadjusted schedule
	 */
	log("Merge my clique\n");

	// Using self unadjusted schedule
	periodTimeToMergeSlotStart.set(periodTimeToMergeMyClique());

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
	periodTimeToMergeSlotStart.set(periodTimeToMergeOtherClique());

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



/*
 * Other clique's sync heard in a fishing slot.
 * Responsibility:
 * - understand superior/inferior i.e. merge my vs merge other
 * - adjust my schedule when merge my
 * - save design so later, at endSyncSlot, we can schedule any mergeSlot.
 */
void CliqueMerger::initFromMsg(SyncMessage* msg){


	assert (MessageFactory::carriesSync(msg->type));
	Logger::logMsgDetail(msg);

	if (owningClique->isOtherCliqueBetter(msg->masterID)) {
		log(Logger::MergeMy);
		initMergeMyClique(msg);
	}
	else {
		log(Logger::MergeOther);
		initMergeOtherClique(msg);
	}

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
	 * deltaSync calculated now.
	 *
	 * deltaSync should be calculated just before sending.
	 *
	 * The receiving unit (mergee) will adjust for latency.
	 */
	DeltaTime deltaSync = owningClique->schedule.deltaNowToNextSyncPoint();

	return MessageFactory::initMergeSyncMessage(deltaSync, superiorMasterID, inferiorMasterID);
}



// Return const pointer to internal data structure, that is constant to the caller
const PeriodTime * CliqueMerger::getPeriodTimeToMergeSlotStart() {
	return &periodTimeToMergeSlotStart;
}
