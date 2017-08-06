
#pragma once

#include <inttypes.h>



/*
 * Transmits Info msg.
 * Usually for logging significant events.
 *
 * Current usage usurps a regular SyncPeriod and transmits in the Sync Slot
 * - high priority
 * - the clique might hear it
 *
 * An alternative usage chooses a different slot:
 * - only a sniffer might hear it
 *
 * Delivery is not guaranteed, now acknowledgement.
 *
 * log() is not synchronous: msg is sent later (queued)
 * trySendingLog() sends previously logged msg
 *
 * log queue does not survive a soft reset
 */

class RemoteLogger {
public:
	static void log(uint8_t);

	/*
	 * If log queue not empty, try sending it, and return true.
	 * When true returned, have performed InfoSlot: disrupts normal SyncPeriod.
	 */
	static bool trySendingLog();

	/*
	 * Send given info.
	 * Performs InfoSlot: disrupts normal SyncPeriod.
	 */
	static void sendInfo(uint8_t);

	/*
	 * Send one indication if any faults logged to UICR flash.
	 * Then you can read the UICR using a debugger probe.
	 * This is called at every reset, before sync loop.
	 */
	static void sendAnyFaults();
};
