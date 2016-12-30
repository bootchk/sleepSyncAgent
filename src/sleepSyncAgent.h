
#include "syncAgent/syncAgent.h"
#include <nRF5x.h>	// Radio, Mailbox, LongClockTimer



/*
 * API of SleepSyncAgent
 *
 * 1. SleepSyncAgent implements an event loop.
 * A call to loopOnEvents() never returns (but does callback.)
 *
 * 2. SleepSyncAgent calls back onSyncPoint() at every SyncPoint.
 *
 * A SyncPoint is end/start of a SyncPeriod.
 * It is generally the same wall (external, real) time (in sync) as SyncPoint of other units.
 *
 * A SyncPoint is not when most Sync messages are exchanged
 * (although it could be concurrent with out-of-sync Sync messages)
 * but it is near the SyncSlot where most Sync messages are exchanged.
 * (That is a design decision, onSyncPoint could be called far from the SyncSlot.)
 *
 * Generally, onSyncPoint() should be kept short.
 * Typically, onSyncPoint() should return within 3mSec (say within 100k instructions?)
 * else it will cut into the SyncSlot and affect Syncing.
 *
 * Since it is not the time when Sync messages are exchanged,
 * onSyncPoint() CAN do a small amount of 'work.'
 * If there is much work, it should be done in a lower priority thread
 * than the SyncAgent thread.
 * (And the SyncAgent algorithm sleep implementation should be changed to a 'yield',
 * cooperating with other threads and only sleeping the mcu if all threads are idle.)
 *
 * Sync among all units is not guaranteed by the callback.
 * Caller can't know for sure that sync is achieved (since units might be mobile)
 * but generally, sync is achieved some time after startup.
 *
 * (In other words, the API has no method to return the quality of sync.
 * In the future, the API could return some quality measure such as the number of merges a unit has done.
 * But unless the network is a mesh network with addressing, there is no quality measure such as:
 * how many units are in sync?)
 *
 * 3. Caller puts mail in Mailbox to broadcast it to all synced units.
 *
 * 4.  SleepSyncAgent calls back onWorkMsg() when a work message is heard from other units.
 * That function runs at the same priority as SleepSyncAgent.
 * It should be short to prevent loss of sync.
 * See the above discussion re 'much work' i.e.
 * much work started by work messages should be handled in a lower priority WorkThread.
 * OnWorkMsg should put work in a queue that unblocks readers (signals the WorkThread.)
 *
 * 5. This wraps (simplifies) the public API of SyncAgent.
 *
 * 5. Parameters of algorithm.
 *
 * The parameters are by default rather loose.
 * For example, a msg takes 3 ticks but a slot is 300 ticks,
 * meaning the 'guard' around sync messages is wide.
 * Narrower guards would take less power.
 *
 * Also, I don't know the maximum number of units that can be synched.
 * To support a higher max (with more contention)
 * the algorithm might change to spread sends across time,
 * and that would narrow the guards.
 *
 * Parameters found in:
 * - scheduleParameters.h
 * - modules/policy/policyParameters.h
 * - in nRFrawProtocol (wireless stack)
 */

class SleepSyncAgent {
public:
	static void init(
			Radio*,
			Mailbox*,
			LongClockTimer*,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);
	static void loopOnEvents() __attribute__ ((noreturn));
};
