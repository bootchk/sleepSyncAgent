
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





void rollPeriodForwardDiscretely() {
	/*
	 * Keep SyncPeriod clock tuned to base LongClock and LFXO,
	 * by rolling period forward by NormalSyncPeriodDuration without regard to nowTime();
	 *
	 * if called late:
	 * yields flawed scheduling (scheduling events in the past or at zero timeout) later
	 * since it leaves the startTimeOfSyncPeriod much in the past.
	 * It yields setting timers for times in the past, which even though they will expire immediately,
	 * the timer expirations will also be late.
	 *
	 * The present design has 40 tick "padding" at the front for HFXO startup.
	 * That padding is sufficent for lateness of less than a few ticks.
	 *
	 * If late, syncPointCallback will be late, but that is not important to sync keeping (only to the app.)
	 * And when the syncPointCallback is called should be changed anyway.
	 */

	 _startTimeOfSyncPeriod = _endTimeOfSyncPeriod;
	 _endTimeOfSyncPeriod = _startTimeOfSyncPeriod + ScheduleParameters::NormalSyncPeriodDuration;

	 Logger::logStartSyncPeriod(_startTimeOfSyncPeriod);

	 // TODO an assertion that we are not too late,
	 // or increment in a loop until start and end time are within limits of now time.
	 /*
	  * !!! This assertion can't be stepped-in while debugging
	  * since the RTC continues to run while you are stepping.
	  */
	 //assert( TimeMath::timeDifferenceFromNow(startTimeOfSyncPeriod) < ScheduleParameters::SlotDuration );
}


} // namespace







void Schedule::init(){
	Logger::log("Schedule reset\n");

	/*
	 * Not necessary: LongClock::resetToNearZero();
	 * This is near POR, the LongClock is still near zero in relation to its near infinite length.
	 */

	_startTimeOfSyncPeriod = LongClock::nowTime();	// Must do this to avoid assertion in rollPeriodForwardToNow
	rollPeriodForward();
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
 */

void Schedule::rollPeriodForward() {
	rollPeriodForwardDiscretely();
}




void Schedule::setEndTimeOfSyncPeriodToNow() {
	_endTimeOfSyncPeriod = LongClock::nowTime();
	/*
	* _startTime is long ago, but we will soon call rollPeriodForward() which calculates _startTime from _endTime
	*/
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
	adjust(adjustedSyncPeriodEndTimeFromMsg(msg));
}


void Schedule::adjust(LongTime newEndTime) {
	LongTime oldEndTimeOfSyncPeriod = _endTimeOfSyncPeriod;

	// FUTURE optimization?? If adjustedEndTime is near old endTime, forego setting it?
	_endTimeOfSyncPeriod = newEndTime;

	// assert old startTimeOfSyncPeriod < new endTimeOfSyncPeriod  < nowTime() + 2*periodDuration

	// endTime never advances backward
	assert(_endTimeOfSyncPeriod > oldEndTimeOfSyncPeriod);

	// end time never jumps too far forward from remembered start time.
	assert( (_endTimeOfSyncPeriod - startTimeOfSyncPeriod()) <= 2* ScheduleParameters::NormalSyncPeriodDuration);
}



void Schedule::adjustByCliqueHistoryOffset(DeltaTime offset){

	adjust(adjustedEndTimeFromCliqueHistoryOffset(offset));
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
 * Adjusted end time of SyncPeriod,
 * where SyncPeriod is never shortened (at all), only extended.
 * Specialized for MasterSync message.
 */
LongTime Schedule::adjustedSyncPeriodEndTimeFromMsg(const SyncMessage* msg) {

	// Get raw time from msg
	LongTime result = adjustedEventTimeFromMsg(msg);

	/*
	 * Don't adjust end time sooner than it already is,
	 * otherwise fishing and merging in this sync period also need adjusting.
	 * If the calculate time is sooner, then add duration of a syncperiod.
	 * So the current sync period will end much later, but that doesn't matter.
	 *
	 * !!!! < or = : if we are already past sync point,
	 * both result and timeOfNextSyncPoint() could be the same
	 * (both deltas are zero.)
	 */
	if (result <= timeOfNextSyncPoint()) {
		result += ScheduleParameters::NormalSyncPeriodDuration;
	}


	Logger::log(result);
	Logger::log("<new period end\n");

	assert( (result - startTimeOfSyncPeriod()) <= 2* ScheduleParameters::NormalSyncPeriodDuration);
	return result;
}


/*
 * Time calculated from msg offset.
 * Msg is either MasterSync or of class MergeSync.
 * Event is either end time of my clique's sync period
 * or start time of other clique's next sync period.
 */
LongTime Schedule::adjustedEventTimeFromMsg(const SyncMessage* msg) {
	/*
	 * Crux: new end time is TOA of SyncMessage + DeltaSync + various delays:
	 * rampup + OTA time + SW overhead.
	 *
	 * Sender has already adjusted offset for SendLatency.
	 *
	 * We don't measure ReceiveLatency here.
	 * The time to perform these calculations is immaterial.
	 * ReceiveLatency is between last OTA bit and time OTA was recorded.
	 */

	DeltaTime delta = msg->deltaToNextSyncPoint.get();
	// delta < SyncPeriodDuration

	// Radio knows toa of single message in use, Radio not enabled while message is processed.
	LongTime toa = Radio::timeOfArrival();

	LongTime result = toa + delta;

	Logger::log(toa);
	Logger::log("<toa\n");
	// Offset already logged in msg details.
	//logInt(deltaNowToNextSyncPoint());
	//log(":Delta to next event\n");
	// log time elapsed since toa, not really used
	//DeltaTime elapsedTicksSinceTOA = TimeMath::clampedTimeDifferenceToNow(getMsgArrivalTime());
	//logInt(elapsedTicksSinceTOA); log("<<<Elapsed since toa.\n");

	return result;
}



LongTime Schedule::adjustedEndTimeFromCliqueHistoryOffset(DeltaTime offset) {
	/*
	 * Assert called at end of sync period.
	 *
	 * Offset is relative to end of sync period of current schedule.
	 */
	LongTime result = LongClock::nowTime() + offset;
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
		Logger::logInt(wrongDelta);
		Logger::log(">>> zero or neg delta to next SyncPoint\n");
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


/*
 * Current design:
 * Allow for HFXO rampup, i.e. not beyond nextSyncPoint - rampup.
 * FUTURE allow to fish last slot if we keep HFXO on and seque directly to SyncSlot.
 *
 * TODO this should also depend on the max duration of slots.
 * E.G. a MergeSlot takes at least one transmission time,
 * and is synchronous so it always takes that time.
 */
LongTime Schedule::latestTimeToEndSlots() {
	// FIXME latesttimetoendslots
	// assert(aTime <= (nextSyncPoint - ScheduleParameters::VirtualSlotDuration + 10*ScheduleParameters::MsgDurationInTicks));
	// TODO does subsequent scheduling prevent past HXFO rampup?
	return timeOfNextSyncPoint();
}


/*
 * Clamp a time.
 *
 * Used for many sync period marks: start and end times of slots, sessions.
 *
 * Clamping of end times may shorten usual durations of slots and sessions.
 *
 * Time to start and end fish slot must be no later than start time of last sleeping slot,
 * else we won't start next sync period on time.
 */
/*
 * Current SyncPeriod is never shortened by adjustment, only lengthened.
 * So that is not a reason why proposedTime might be before next sync point.
 */


LongTime Schedule::clampTimeBeforeLatestSlotMark(LongTime proposedTime) {

	LongTime result = proposedTime;
	LongTime cutoffTime = latestTimeToEndSlots();

	if (result > cutoffTime) {
		Logger::log("Clamp time before latest slot\n");
		result = cutoffTime;
	}
	return result;
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
