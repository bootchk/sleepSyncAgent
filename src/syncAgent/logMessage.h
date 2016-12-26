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

	static constexpr const char* FishedMasterSync = "Fished Master\n";
	static constexpr const char* FishedMergeSync = "Fished Merge\n";
	static constexpr const char* FishedWorkSync = "Fished Work\n";

	// Sending
	static constexpr const char* SendMasterSync = "TX Master\n";
	static constexpr const char* SendWorkSync = "TX Work\n";
	static constexpr const char* SendMergeSync = "TX Merge\n";
	static constexpr const char* SendWork = "TX Work\n";
	static constexpr const char* SendNone = "Listen\n";

	static void logStartSyncPeriod(LongTime now) {
		return;
		logLongLong(now);
		log("<Sync\n");
	}
};
