
#include "flashLogger.h"

#include <services/customFlash.h>

#include "flashIndex.h"

//#include "../globals.h" // clique


// Functions write to flash memory (when not connected to debugger hw)
// FUTURE put this in some Handler to see?  But I already know what handlers are called.
//#include "app_util_platform.h"
// uint32_t ipsr = __get_IPSR();

// Exhausted power while executing slot sequence
void FlashLogger::logNoPowerToFish()          { CustomFlash::writeZeroAtIndex(NoPowerToFish); }
void FlashLogger::logNoPowerToStartSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToStartSyncSlot); }
void FlashLogger::logNoPowerForHalfSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToHalfSyncSlot); }
void FlashLogger::logOverslept()              { CustomFlash::writeZeroAtIndex(OverSlept); }

// Only a timer running but it was not the reason for wake.
void FlashLogger::logUnexpectedWakeReason()   { CustomFlash::writeZeroAtIndex(UnexpectedWake); }

void FlashLogger::logUnexpectedMsg()          { CustomFlash::writeZeroAtIndex(UnexpectedMsg); }


/*
 * All our IRQ's set reason.
 * Except that we use the PPI without ISR, so we DO get waking events without an ISR running.
 *
 * Maybe a default IRQ ran (without setting reason)
 * or any other unexpected return from WFE.
 */
void FlashLogger::logWakeWithoutIRQSettingReason() {
	//localLogger.log("\nWake w/o known IRQ set reason.");
	CustomFlash::writeZeroAtIndex(UnexpectedWakeWhileListen);
}
