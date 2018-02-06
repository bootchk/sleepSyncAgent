

#include "syncWorkSlot.h"

#include "../../modules/syncBehaviour.h"
#include "../../policy/workManager.h"

/*
 * Sub types of syncWorkSlot
 *
 * Behavior varies according to what needs to be sent.
 */


void SyncWorkSlot::dispatchSyncSlotKind() {
	// Call shouldTransmitSync every time, since it needs calls side effect reset itself
	bool needXmitSync = SyncBehaviour::shouldTransmitSync();

	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood airwaves and destroy sync.
	 * (colliding too often with MergeSync or MasterSync.)
	 *
	 * Owner of SyncAgent decides policy and conveys  (queues) work to be sent.
	 * OBSOLETE: WorkPolicy::shouldXmitWorkSync() and
	 */
	if (WorkManager::isNeedSendWork()) {
		// This satisfies needXmitSync
		doSendingWorkSyncWorkSlot();
	}
	else {
		// No work to send, maintain sync if master
		if (needXmitSync)
			doMasterSyncWorkSlot();
		else
			/*
			 * isSlave
			 * OR (isMaster and not xmitting (coin flip))
			 */
			doSlaveSyncWorkSlot();
	}

}
