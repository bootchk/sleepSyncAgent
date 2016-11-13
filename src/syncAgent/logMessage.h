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
	static constexpr const char* MasterSync = "MasterSync\n";
	static constexpr const char* WorseMasterSync = "WorseMasterSync\n";
	static constexpr const char* MergeSync = "MergeSync\n";
	static constexpr const char* AbandonMastership = "AbandonMastership\n";
	static constexpr const char* Work = "Work\n";

	// Sending
	static constexpr const char* SendMasterSync = "SendMasterSync\n";
	static constexpr const char* SendWork = "SendWork\n";
	static constexpr const char* SendMergeSync = "SendMergeSync\n";
};
