
#include "sleepDuration.h"

#include "../scheduleParameters.h"

// clique knows sync slot timing
#include "../clique/clique.h"
#include "../globals.h"  // clique

// fishSchedule knows fish slot timing
#include "../slots/fishing/fishSchedule.h"

// lib embeddedMath
#include <timeMath.h>


DeltaTime  SleepDuration::powerCheckInterval() {
	return ScheduleParameters::TimeoutWaitingForSyncPowerSleeper;
}

DeltaTime SleepDuration::nowTilSyncPoint() {
	return clique.schedule.deltaNowToNextSyncPoint();
}

DeltaTime SleepDuration::nowTilFishStart() {
	return FishSchedule::deltaToFishSessionStart();
}

DeltaTime SleepDuration::nowTilPreludeWSync() {
	return TimeMath::deltaSubtraction( nowTilSyncPoint(), preludeTilNextTask());
}

DeltaTime SleepDuration::nowTilPreludeWFish() {
	return TimeMath::deltaSubtraction(nowTilFishStart(), preludeTilNextTask());
}

/*
 * Prelude takes less time, but is convenient to sleep a whole slot.
 *
 * Here we wait a time that with high probablity prelude is complete.
 * If it is not complete, the radio won't work, but we carry on as if it is working.
 * Another design would get an event from prelude completed.
 */
DeltaTime SleepDuration::preludeTilNextTask() {
	return ScheduleParameters::VirtualSlotDuration;
}
