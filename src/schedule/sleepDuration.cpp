
#include "sleepDuration.h"

#include "../scheduleParameters.h"


DeltaTime SleepDuration::nowTilSyncPoint() {

}

DeltaTime SleepDuration::nowTilFishStart() {

}

DeltaTime SleepDuration::nowTilPreludeWSync() {
	return nowTilSyncPoint() - preludeTilNextTask();
}

DeltaTime SleepDuration::nowTilPreludeWFish() {
	return nowTilFishStart() - preludeTilNextTask();
}

/*
 * Prelude takes less time, but is convenient to sleep a whole slot.
 *
 * Here we wait a time that with high probablity prelude is complete.
 * If it is not complete, the radio won't work, but we carry on as if it is working.
 * Another design would get an event from prelude completed.
 */
DeltaTime SleepDuration::preludeTilNextTask() {
	return ScheduleParameters::RealSlotDuration;
}
