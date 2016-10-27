
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

	// logged in dispatch messages
	static constexpr const char* MasterSync = "MasterSync\n";
	static constexpr const char* WorseMasterSync = "WorseMasterSync\n";
	static constexpr const char* MergeSync = "MergeSync\n";
	static constexpr const char* AbandonMastership = "AbandonMastership\n";
	static constexpr const char* Work = "Work\n";

	// Misc
	static constexpr const char* SendWork = "SendWork\n";
};
