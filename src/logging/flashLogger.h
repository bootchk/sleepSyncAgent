
#pragma once

/*
 * Log to persistent memory: flash, UICR
 *
 * Not compatible with SD?
 */

class FlashLogger {
public:
	// Exhausted power while executing slot sequence
		static void logNoPowerToFish();
		static void logNoPowerToStartSyncSlot();
		static void logNoPowerForHalfSyncSlot();

		static void logOverslept();

		// Only a timer running but it was not the reason for wake.
		static void logUnexpectedWakeReason();

		static void logUnexpectedMsg();
		static void logWakeWithoutIRQSettingReason();
};
