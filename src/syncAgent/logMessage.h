#pragma once

/*
 * Holds log messages.
 */
class LogMessage {
public:
	// logged at start of slots
	static constexpr const char* SyncSlot = "SyncSlot\n";
	static constexpr const char* WorkSlot = "WorkSlot\n";
	static constexpr const char* FishSlot = "FishSlot\n";
	static constexpr const char* MergeSlot = "MergeSlot\n";

	// logged in dispatch messages received
	static constexpr const char* RXMasterSync = "  RX Master\n";
	static constexpr const char* RXWorseMasterSync = "  RX WorseMaster\n";
	static constexpr const char* RXMergeSync = "  RX Merge\n";
	static constexpr const char* RXAbandonMastership = "  RX AbandonMastership\n";
	static constexpr const char* RXWorkSync = "  RX Work\n";
	static constexpr const char* RXUnknown = "  RX unknown msg type\n";

	static constexpr const char* FishedMasterSync = "Fish Master\n";
	static constexpr const char* FishedMergeSync = "Fish Merge\n";
	static constexpr const char* FishedWorkSync = "Fish Work\n";
	static constexpr const char* FishedAbandonMastershipSync = "Fish Abandon\n";

	// Sending
	static constexpr const char* SendMasterSync = "TX Master\n";
	static constexpr const char* SendWorkSync = "TX Work\n";
	static constexpr const char* SendMergeSync = "TX Merge\n";
	static constexpr const char* SendWork = "TX Work\n";
	static constexpr const char* SendAbandonMastership = "TX Abandon\n";
	static constexpr const char* SendNone = "Listen\n";

	// Role change
	static constexpr const char* ToFisher = "Role fisher\n";
	static constexpr const char* ToMerger = "Role merger\n";

	// Message statistics
	static constexpr const char* Garbled = "Msg type or offset garbled\n";
	static constexpr const char* CRC = "Msg CRC\n";

	static void logStartSyncPeriod(LongTime now) {
		return;
		logLongLong(now);
		log("<Sync\n");
	}



	// Functions write to flash memory (when not connected to debugger hw)
	// FUTURE put this in some Handler to see?  But I already know what handlers are called.
	//#include "app_util_platform.h"
	// uint32_t ipsr = __get_IPSR();

	// Exhausted power while executing slot sequence
	static void logNoPowerToFish() { CustomFlash::writeZeroAtIndex(NoPowerToFish); }
	static void logNoPowerToStartSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToStartSyncSlot); }
	static void logNoPowerForHalfSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToHalfSyncSlot); }

	// Only a timer running but it was not the reason for wake.
	static void logUnexpectedWakeReason() { CustomFlash::writeZeroAtIndex(UnexpectedWake); }

	static void logUnexpectedMsg() { CustomFlash::writeZeroAtIndex(UnexpectedMsg); }
	static void logUnexpectedWakeWhileListening() { CustomFlash::writeZeroAtIndex(UnexpectedWakeWhileListen); }

	static void logStartSync() { CustomFlash::writeZeroAtIndex(StartSync); }
	static void logPauseSync() { CustomFlash::writeZeroAtIndex(PauseSync); }

	static void logListenHalfSlot() { CustomFlash::writeZeroAtIndex(ListenHalf); }
	static void logListenFullSlot() { CustomFlash::writeZeroAtIndex(ListenFull); }
	static void logFished() { CustomFlash::writeZeroAtIndex(LogFished); }

};
