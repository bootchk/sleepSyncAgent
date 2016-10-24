
#include "syncAgent/syncAgent.h"
#include "platform/platform.h"	// Radio



/*
 * API of SleepSyncAgent
 *
 * SleepSyncAgent implements an event loop.
 * A call to loopOnEvents() never returns.
 *
 * SleepSyncAgent calls back onSyncPoint() at every SyncPoint.
 * Sync among all units is not guaranteed by the callback.
 * Caller has no way of knowing when sync is achieved.
 *
 * Caller puts mail in Mailbox to broadcast it to all synced units.
 *
 * SleepSyncAgent calls back onWorkMsgQueued() when a work message is queued.
 * That function runs at the same priority as SleepSyncAgent.
 * It should be short to prevent loss of sync.
 * Work messages should be handled in a lower priority thread (say WorkThread.)
 * If the queue does not unblock readers, onWorkMsgQueued()
 * should signal the WorkThread.
 *
 * This wraps (simplifies) the public API of SyncAgent.
 *
 * Parameters of algorithm:
 * - scheduleParameters.h
 * - modules/policy/policyParameters.h
 * - in nRFrawProtocol (wireless stack)
 */

class SleepSyncAgent {
public:
	static void init(
			Radio*,
			Mailbox*,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);
	static void loopOnEvents();	// never returns
};
