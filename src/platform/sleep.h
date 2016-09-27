
#include "types.h"	// OSTIme

/*
 * Sleep until a waking event occurs, or until timeout.
 *
 * Waking event includes:
 * - radio msg received (if radio is on.)
 * - Timer expired
 *
 * !!! SleepSync expects no other events.
 * SyncAgent not currently equipped to handle e.g. GPIO e.g. button press events.
 *
 *
 * Sleep is low power.
 * Platform must put all unused peripherals into low power mode.
 * (Automatic on some peripherals.)
 *
 * Typically, platform's RTC peripheral is NOT powered off, provides Timer.
 *
 * Platform must not put radio peripheral into low-power if SyncAgent has powered it.
 */
void sleepUntilEventWithTimeout(OSTime);

/*
 * Return true if reason for end of sleep is as stated.
 *
 * Usually implemented by returning a flag set in an ISR for event that woke.
 */
bool reasonForWakeIsMsgReceived();
bool reasonForWakeIsTimerExpired();

// TODO brownout and faults?
