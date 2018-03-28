
// Part of SSTask class implementation, specific to merging
#include "task.h"

#include "../modules/syncSender.h"
#include "../slots/merging/mergePolicy.h"
//#include "../syncAgentImp/state/role.h"
#include "../syncAgentImp/syncAgentImp.h"

#include "../logging/logger.h"


/*
 * Start send, and check completion.
 */
void SSTask::mergerStart() {
	// TODO unimp
	Logger::logTicksSinceStartSyncPeriod();

	SyncSender::sendMergeSync();	// Synchronous, i.e. spins until done

	if (MergePolicy::checkCompletionOfMergerRole()){
		SyncAgentImp::stopMerger();
	}

	/*
	 * When above is synchronous, next scheduled task is PreludeWSync
	 *
	 *  FUTURE above is asynchronous, next scheduled task is mergerEnd, to turn off HFXO.
	 *  We can set up a PPI to do that, but then PPI takes energy?
	 */
}
