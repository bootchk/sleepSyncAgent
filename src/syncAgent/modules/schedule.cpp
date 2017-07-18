
#include <cassert>

#include "schedule.h"

#include "../logging/logger.h"
#include "../scheduleParameters.h"	// probably already included by MergeOffset


namespace {

/*
 * Set every SyncPoint (when SyncPeriod starts) and never elsewhere.
 * I.E. It is history that we don't rewrite.
 * Invariant: in the past
 */
LongTime _startTimeOfSyncPeriod;

/*
 * Set every SyncPoint (when SyncPeriod starts) to normal end time
 * !!! but also might be adjusted further into the future
 * i.e. period extended
 * on hearing MasterSync or MergeSync.
 *
 * !!! Can change during current period.  See adjustBySyncMsg()
 * Can be in future, advances forward in time.
 *
 * A property, with a getter that should always be used
 * in case we want to migrate calculations to the getter.
 */
LongTime _endTimeOfSyncPeriod;


} // namespace







void Schedule::init(){
	log("Schedule reset\n");

	/*
	 * Not necessary: LongClock::resetToNearZero();
	 * This is near POR, the LongClock is still near zero in relation to its near infinite length.
	 */

	_startTimeOfSyncPeriod = LongClock::nowTime();	// Must do this to avoid assertion in rollPeriodForwardToNow
	rollPeriodForwardToNow();
	// Out of sync with other cliques
}


/*
 * Called at wall time that should be SyncPoint.
 * That is, called when a timer expires that indicates end of sync period and time to begin next.
 *
 * There are cases when this is called late:
 * 1) Since we can fish in the last slot before this time, and fishing code may delay us a short time,
 * this may be called a short time later than usual.
 * If not a short time, is error in algorithm.
 *
 * 2) There could be other faults that delay a call to this (timer does not expire at correct wall time.)
 * E.G. there is a mysterious delay when an invalid CRC is detected.
 *
 * 3) Also, using the debugger could delay timer expiration.
 *
 * In those cases, if the delay is not short, sync is lost
 * (the wall time of our SyncPoint not the same as other clique member's wall time of SyncPoint.)
 * But it is unavoidable for a robust algorithm.  See next paragraph.
 *
 * FLAWED design if called late:
 *    startTimeOfSyncPeriod = endTimeOfSyncPeriod;
 *    endTimeOfSyncPeriod = startTimeOfSyncPeriod + NormalSyncPeriodDuration;
 * That yields flawed scheduling (scheduling events in the past or at zero timeout) later
 * since it leaves the startTimeOfSyncPeriod much in the past.
 * It yields setting timers for times in the past, which should expire immediately.
 */
void Schedule::rollPeriodForwardToNow() {

	//LongTime startOfPreviousSyncPeriod = startTimeOfSyncPeriod;

	LongTime now = LongClock::nowTime();

	Logger::logStartSyncPeriod(now);

	// Starts now.  See above.  If called late, sync might be lost.
	_startTimeOfSyncPeriod = now;
	_endTimeOfSyncPeriod = now + ScheduleParameters::NormalSyncPeriodDuration;

	/*
	 * assert startTimeOfSyncPeriod is close to nowTime().
	 * This is called at the time that should be SyncPoint.
	 * But since we can fish in the last slot before this time,
	 * and fishing may delay us a short time,
	 * this may be called a short time later than usual.
	 * If not a short time, is error in algorithm.
	 */
	/*
	 * !!! This assertion can't be stepped-in while debugging
	 * since the RTC continues to run while you are stepping.
	 */
	//assert( TimeMath::timeDifferenceFromNow(startTimeOfSyncPeriod) < ScheduleParameters::SlotDuration );
}


/*
 * Crux
 *
 * Called from a sync, work, or fish slot:
 * - most often, usually, a MasterSync in a Sync slot
 * - rarely, a MergeSync in Sync slot
 * - very rarely, a MasterSync in a Fish slot
 *
 * A sync message ADDS to ***end*** of period (farther into the future).
 * Except for a MasterSync received in SyncSlot by unit that is lagging,
 * when the MasterSync may SUBTRACT, i.e.  advance syncpoint closer to now.
 *
 * Otherwise, for a fish slot, the calculation of the end of the fish slot
 * (based on the start) might be beyond the end of sync period?
 * (But a fish slot ends on the first msg, so not a concern?)
 *
 * It is not trivial to schedule end of period in the future,
 * since we might be near the current end of the period already.
 * assert startTimeOfSyncPeriod < nowTime()  < endTimeOfSyncPeriod
 * i.e. now in the current period, but may be near the end of it,
 * or beyond it because of delay in processing fished SyncMessage?
 */
void Schedule::adjustBySyncMsg(SyncMessage* msg) {
	/*
	 * assert endSyncSlot or endFishSlot has not yet occurred, but this doesn't affect that.
	 */

	LongTime oldEndTimeOfSyncPeriod = _endTimeOfSyncPeriod;

	// FUTURE optimization?? If adjustedEndTime is near old endTime, forego setting it?
	_endTimeOfSyncPeriod = adjustedEndTime(msg);

	// assert old startTimeOfSyncPeriod < new endTimeOfSyncPeriod  < nowTime() + 2*periodDuration

	// endTime never advances backward
	assert(_endTimeOfSyncPeriod > oldEndTimeOfSyncPeriod);

	// end time never jumps too far forward from remembered start time.
	assert( (_endTimeOfSyncPeriod - startTimeOfSyncPeriod()) <= 2* ScheduleParameters::NormalSyncPeriodDuration);
}

/*
 * ALTERNATIVE DESIGN: NOT IMPLEMENTED
 * Adjusted end time of SyncPeriod, where SyncPeriod is never shortened by much, only lengthened.
 * "By much" means: not more than one slot before current end time of SyncPeriod.
 * Here, elsewhere there must be code to insure that fishing in the last slot
 * and MergeSync transmit does not fall outside the shortened SyncPeriod.


	DeltaTime adjustment;
	if (senderDeltaToSyncPoint > SlotDuration) {
		// Self is not already near end of adjusted SyncPeriod
		adjustment = senderDeltaToSyncPoint;
 */
/*
 * Adjusted end time of SyncPeriod, where SyncPeriod is never shortened (at all), only lengthened.
 */
LongTime Schedule::adjustedEndTime(const SyncMessage* msg) {

	// log time elapsed since toa, not really used
	//DeltaTime elapsedTicksSinceTOA = TimeMath::clampedTimeDifferenceToNow(getMsgArrivalTime());
	//logInt(elapsedTicksSinceTOA); log("<<<Elapsed since toa.\n");

	/*
	 * Crux: new end time is TOA of SyncMessage + DeltaSync + various latencies
	 */
	DeltaTime delta = msg->deltaToNextSyncPoint.get();
	// delta < SyncPeriodDuration

	LongTime toa = msg->timeOfArrival;
	LongTime result = toa +
			+ delta
			- ScheduleParameters::MsgOverTheAirTimeInTicks
			- ScheduleParameters::SenderLatency;

	/*
	 * Don't adjust end time sooner than it already is,
	 * otherwise fishing and merging in this sync period also need adjusting.
	 *
	 * !!!! < or = : if we are already past sync point,
	 * both result and timeOfNextSyncPoint() could be the same
	 * (both deltas are zero.)
	 */
	if (result <= timeOfNextSyncPoint()) {
		result += ScheduleParameters::NormalSyncPeriodDuration;
	}

	// -DLOGGING
	logLongLong(toa); log(":toa\n");
	logInt(delta); log(":offset\n");
	logLongLong(result); log(":new period end\n");
	//logInt(deltaNowToNextSyncPoint()); log(":Delta to next sync\n");

	assert( (result - startTimeOfSyncPeriod()) <= 2* ScheduleParameters::NormalSyncPeriodDuration);
	return result;
}


LongTime Schedule::startTimeOfSyncPeriod(){ return _startTimeOfSyncPeriod; }
LongTime Schedule::endTimeOfSyncPeriod(){ return _endTimeOfSyncPeriod; }





DeltaTime  Schedule::deltaNowToNextSyncPoint() {
	DeltaTime result = TimeMath::clampedTimeDifferenceFromNow(timeOfNextSyncPoint());
	/*
	 * Usually next SyncPoint is future.
	 * If we are already past it, will whack sync.
	 * It could happen if we fish in the last slot and code delays us past nextSyncPoint.
	 */
#ifndef NDEBUG
	// Code for testing only
	if (result == 0) {
		// Calculate magnitude in past, how late are we?
		DeltaTime wrongDelta = TimeMath::elapsed(timeOfNextSyncPoint());
		logInt(wrongDelta);
		log(">>> zero or neg delta to next SyncPoint\n");
	}
#endif
	return result;
}

/*
 * Different: backwards from others: from past time to now
 *
 * This can be called when now is after current sync period,
 * hence result can be greater than sync period duration.
 * Stronger would return PeriodTime.
 */
DeltaTime  Schedule::deltaPastSyncPointToNow() {
	DeltaTime result = TimeMath::clampedTimeDifferenceToNow(startTimeOfSyncPeriod());

	// Not:  assert(result <= ScheduleParameters::NormalSyncPeriodDuration);
	return result;
}




// Next
LongTime Schedule::timeOfNextSyncPoint() {
	return _endTimeOfSyncPeriod;
}






#ifdef OBSOLETE

/*
 * The offset a Work message would hold (if offset were not used otherwise)
 * Used to mangle a Work message to be equivalent to a Sync.
 * A Work message is sent 1-1/2 slots from SyncPoint.
 */
DeltaTime Schedule::deltaFromWorkMiddleToEndSyncPeriod(){
	return ScheduleParameters::NormalSyncPeriodDuration - ScheduleParameters::SlotDuration - halfSlotDuration();
}


// WorkSlot immediately after SyncSlot
LongTime Schedule::timeOfThisWorkSlotMiddle() { return startTimeOfSyncPeriod() + ScheduleParameters::SlotDuration + halfSlotDuration(); }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfSyncPeriod() + 2 * ScheduleParameters::SlotDuration; }

// We don't need WorkSlot start
DeltaTime Schedule::deltaToThisWorkSlotMiddle(){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisWorkSlotMiddle());
}
DeltaTime Schedule::deltaToThisWorkSlotEnd(){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd());
}



//LongTime Schedule::timeOfNextSyncSlotStart() { return timeOfNextSyncPeriodStart(); }


/*
 * Duration of this SyncPeriod, possibly as adjusted.
 */
DeltaTime Schedule::thisSyncPeriodDuration() {

}
#endif
