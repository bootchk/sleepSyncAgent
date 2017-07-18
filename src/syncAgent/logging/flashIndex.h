#pragma once


/*
 * Index of debugging flags written to words in UICR.
 *
 * Typically use word as a flag for certain exceptions and other events.
 * A flag is 'set' by writing all zeroes to it.
 *
 * Exceptions are handled generically, writing PC to flash.
 * Flagged events are specific to the application.
 */
enum AppFlagsIndex {
	// First three are reserved by nRF5x library
	//HardFaultPCIndex = 0,			// hw fault PC
	//ExitFaultIndex,
	//LineNumberIndex, 				// line no of assert

	// Normal algorithm step (Phase) written to flash on a fault or brownout warning
	PhaseBOIndex = 3,						// phase of algorithm when fault or brownout occurred
	// Bits per phase written first time phase is done
	PhasesDoneIndex,
	//
	/*
	 *  Flags for exceptions discovered in the code, for robustness.
	 *  Alternative to C++ exception mechanism.
	 */
	UnexpectedWake,					// sleep ended but timer not expired
	UnexpectedMsg,					// Radio IRQ while radio disabled?
	UnexpectedWakeWhileListen,		// radio on but woken for unknown reason

	/*
	 * Rare but expected events.
	 */
	ExcessPowerEventFlagIndex,		// Vcc above 3.6V
	//WorkEventFlagIndex,				// Worked e.g. flashed LED
	NoPowerToFish,					// Vcc fell below 2.5V
	NoPowerToStartSyncSlot,			//    "
	NoPowerToHalfSyncSlot,			//  "
	PauseSync,					    // not enough power to listen/send sync
	OverSlept,						//
	IntendedSleepDuration,
	OversleptDuration,			    // overslept sleep time

	// These should not exceed 16, rest reserved for brownout traces and assertion string
};
