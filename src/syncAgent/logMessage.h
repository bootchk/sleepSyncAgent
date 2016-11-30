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
	static constexpr const char* MasterSync = "RX MasterSync\n";
	static constexpr const char* WorseMasterSync = "RX WorseMasterSync\n";
	static constexpr const char* MergeSync = "RX MergeSync\n";
	static constexpr const char* AbandonMastership = "RX AbandonMastership\n";
	static constexpr const char* WorkSync = "RX WorkSync\n";
	static constexpr const char* FishedWork = "RX FishedWork\n";

	// Sending
	static constexpr const char* SendMasterSync = "TX MasterSync\n";
	static constexpr const char* SendWorkSync = "TX WorkSync\n";
	static constexpr const char* SendMergeSync = "TX MergeSync\n";
	static constexpr const char* SendWork = "TX Work\n";
	static constexpr const char* SendNone = "Listen\n";

};
