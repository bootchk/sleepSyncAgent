#pragma once

/*
 * Understands how to sleep until a certain time in schedule.
 */

/*
 * All methods are deep sleep.
 * Require ensemble shutdown (especially HFXO not running.)
 *
 * All methods are actually "sleep remainder" i.e. can be called after start of sync period or slot.
 */
class ScheduleSleeper {
public:
	static void sleepUntilSyncSlotMiddle();
	static void sleepSyncSlotRemainder();

	static void sleepEntireSyncPeriod();
	static void sleepRemainderOfSyncPeriod();

	static void sleepUntilProvisionSlot();
};
