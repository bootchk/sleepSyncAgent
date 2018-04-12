
#include "scheduleSleeper.h"

#include "../slots/syncing/syncSlotSchedule.h"
#include "../globals.h"	// clique
#include "../clique/clique.h"	// clique
#include "../syncAgentImp/state/phase.h"
#include "syncSleeperObs.h"


void ScheduleSleeper::sleepUntilSyncSlotMiddle() {
	SyncSleeper::sleepUntilTimeout(
			SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot
	);
}

void ScheduleSleeper::sleepEntireSyncPeriod() {
	Phase::set(PhaseEnum::SleepEntireSyncPeriod);
	SyncSleeper::sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}

void ScheduleSleeper::sleepSyncSlotRemainder() {
	//assert(!ensemble->isInUse());
	SyncSleeper::sleepUntilTimeout(SyncSlotSchedule::deltaToThisSyncSlotEnd);
}

void ScheduleSleeper::sleepRemainderOfSyncPeriod() {
	SyncSleeper::sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}

void ScheduleSleeper::sleepUntilProvisionSlot() {
	// TODO random sleep time
}
