
#include "syncSlotSchedule.h"

#include "../../scheduleParameters.h"

#include "../../clique/clique.h"
#include "../../globals.h"  // clique


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
#ifdef TASKS
	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::DeltaSyncSlotStartToSyncSlotXmit;
#else
	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::DeltaSyncPointToSyncSlotMiddle;
#endif
}


/*
 * Real slot is longer than virtual slot,
 * by startup delays for ensemble.
 */
LongTime SyncSlotSchedule::timeOfThisSyncSlotEnd() {
#ifdef TASKS
	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::VirtualSlotDuration;
#else
	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::RealSlotDuration;		// !!!!
#endif
}

