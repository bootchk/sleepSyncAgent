
#include "syncSlotSchedule.h"

#include "../../scheduleParameters.h"

#include "../../clique/clique.h"
#include "../../globals.h"  // clique

// radioSoC
#include <clock/clockDuration.h>


DeltaTime SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot(){
	return ClockDuration::clampedTimeDifferenceFromNow(timeOfThisSyncSlotMiddleSubslot());
}

DeltaTime SyncSlotSchedule::deltaToThisSyncSlotEnd(){
	return ClockDuration::clampedTimeDifferenceFromNow(timeOfThisSyncSlotEnd());
}

/*
 * Start of period and start of SyncSlot coincide.
 * FUTURE: Choice of sync slot at start of period is arbitrary, allow it to be anywhere in period?
 */



LongTime SyncSlotSchedule::timeOfThisSyncSlotMiddleSubslot() {

	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::DeltaSyncSlotStartToSyncSlotXmit;
	// SANS PRELUDE return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::DeltaSyncPointToSyncSlotMiddle;
}


/*
 * Real slot is longer than virtual slot,
 * by startup delays for ensemble.
 */
LongTime SyncSlotSchedule::timeOfThisSyncSlotEnd() {
	return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::VirtualSlotDuration;
	//SANS PRELUDE return clique.schedule.startTimeOfSyncPeriod()  +   ScheduleParameters::RealSlotDuration;		// !!!!
}

