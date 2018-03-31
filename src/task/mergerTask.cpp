
// Part of SSTask class implementation, specific to merging
#include "task.h"

#include "../modules/syncSender.h"
#include "../syncAgentImp/syncAgentImp.h"
#include "../schedule/syncSchedule.h"

#include "../logging/logger.h"
#include "../slots/merge/mergePolicy.h"


/*
 * Merger is one task (not separate start and end tasks)
 *
 * start send
 * wait
 * check completion
 * schedule next task
 */
void SSTask::mergerStartAndEnd() {
	Logger::logTicksSinceStartSyncPeriod();

	SyncSender::sendMergeSync();	// Synchronous, i.e. spins until done

	if (MergePolicy::checkCompletionOfMergerRole()){
		SyncAgentImp::stopMerger();
	}
	// radio not active but RadioPrelude active

	/*
	 * When above is synchronous, next scheduled task is PreludeWSync
	 *
	 *  FUTURE above is asynchronous, next scheduled task is mergerEnd, to turn off HFXO.
	 *  We can set up a PPI to do that, but then PPI takes energy?
	 */
	SyncSchedule::syncSlotAfterMerging();
}
