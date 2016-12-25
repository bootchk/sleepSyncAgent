
#include "syncSlotSchedule.h"

#include "../globals.h"  // clique, fishPolicy

DeltaTime SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot(){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisSyncSlotMiddleSubslot());
}

DeltaTime SyncSlotSchedule::deltaToThisSyncSlotEnd(){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisSyncSlotEnd());
}

/*
 * Start of period and start of SyncSlot coincide.
 * FUTURE: Choice of sync slot at start of period is arbitrary, allow it to be anywhere in period?
 */



LongTime SyncSlotSchedule::timeOfThisSyncSlotMiddleSubslot() {
	return clique.schedule.startTimeOfSyncPeriod()
			+ ScheduleParameters::DeltaToSyncSlotMiddle;
}


/*
 * Real slot is longer than virtual slot,
 * by startup delays for radio.
 */
LongTime SyncSlotSchedule::timeOfThisSyncSlotEnd() {
	return clique.schedule.startTimeOfSyncPeriod()
			+ ScheduleParameters::RealSlotDuration;		// !!!!
}

