
#include "syncAgent/syncAgent.h"
#include "platform/platform.h"	// Radio



/*
 * API of SleepSyncAgent
 *
 * SleepSyncAgent implements an event loop.
 * A call to loopOnEvents() never returns.
 *
 * SleepSyncAgent calls onWorkMsgQueued() when a work message is queued.
 * That function runs at the same priority as SleepSyncAgent.
 * It should be short to prevent loss of sync.
 * Work messages should be handled in a lower priority thread (say WorkThread.)
 * If the queue does not unblock readers, onWorkMsgQueued()
 * should signal the WorkThread.
 *
 * This wraps the public API of a larger object.
 *
 * Parameters of algorithm:
 * - scheduleParameters.h
 * - modules/policy/policyParameters.h
 * - in nRFrawProtocol (wireless stack)
 */

// FUTURE pass the queue.  For now, there is no queue, just the signal.

class SleepSyncAgent {
private:
	// instance of wrapped class
	static SyncAgent syncAgent;

public:
	static void init(
			Radio* radio,
			void (*onWorkMsg)(WorkPayload)
			);
	static void loopOnEvents();	// never returns
};
