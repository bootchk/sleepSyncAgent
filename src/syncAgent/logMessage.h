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
	static constexpr const char* RXMasterSync = "RX MasterSync\n";
	static constexpr const char* RXWorseMasterSync = "RX WorseMasterSync\n";
	static constexpr const char* RXMergeSync = "RX MergeSync\n";
	static constexpr const char* RXAbandonMastership = "RX AbandonMastership\n";
	static constexpr const char* RXWorkSync = "RX WorkSync\n";
	static constexpr const char* RXUnknown = "RX unknown msg type\n";

	static constexpr const char* FishedMasterSync = "Fished MasterSync\n";
	static constexpr const char* FishedMergeSync = "Fished MergeSync\n";
	static constexpr const char* FishedWorkSync = "Fished WorkSync\n";

	// Sending
	static constexpr const char* SendMasterSync = "TX MasterSync\n";
	static constexpr const char* SendWorkSync = "TX WorkSync\n";
	static constexpr const char* SendMergeSync = "TX MergeSync\n";
	static constexpr const char* SendWork = "TX Work\n";
	static constexpr const char* SendNone = "Listen\n";

};
