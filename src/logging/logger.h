#pragma once

#include "../message/message.h"


enum RemoteLoggedEvent {
	StartSync,
	StopSync,
	StartFish,
	StopFish,
	MasterDropOut,
	BrownOut,
	HardOrAssertionFault,
	FishDurationInc,
	FishDurationDec,
	FishDurationMin
};



/*
 * Abstract Logger w/ some specificity to SleepSyncAgent.
 *
 * Calls another Logger from some library.
 */
class Logger {
public:

	static void init();

	// logged at start of slots
	static constexpr const char* SyncSlot = "SyncSlot\n";
	static constexpr const char* WorkSlot = "WorkSlot\n";
	static constexpr const char* FishSlot = "FishSlot\n";
	static constexpr const char* MergeSlot = "MergeSlot\n";

	// Fished but ignored
	static constexpr const char* FishedMergeSync = "Fished merge msg.\n";
	static constexpr const char* FishedAbandonMastershipSync = "Fish abandon msg.\n";

	static constexpr const char* MergeMy = "\nMergeMy.";
	static constexpr const char* MergeOther = "\nMergeOther.";

	static constexpr const char* SendNone = "Listen\n";

	// Role change
	static constexpr const char* ToFisher = "\nRole fisher.";
	static constexpr const char* ToMerger = "\nRole merger.";
	static constexpr const char* ToNoFish = "\nRole no fish.";

	// Message statistics
	static constexpr const char* Garbled = "Msg type or offset garbled.\n";
	static constexpr const char* CRC = "Msg CRC\n";


	static void logSystemInfo();
	static void logStartSyncPeriod(LongTime now);


	/*
	 * Unexpected, rare events.
	 */
	static void logWakeWithoutIRQSettingReason();
	static void logWakeIRQFoundNoEvent();
	static void logUnexpectedEventWhileListening();

	static void logIncreaseFish();
	static void logDecreaseFish();
	static void logToMinFish();

	/*
	 * Normal details of algorithm.
	 */
	// Message related
	static void logReceivedMsg(SyncMessage* msg);
	static void logSend(SyncMessage* msg);
	static void logMsgDetail(SyncMessage* msg);

	static void logTicksSinceStartSyncPeriod();

	static void logInferiorCliqueSyncSlotOfMaster();
	static void logInferiorCliqueSyncSlotOfSlave();


	/*
	 * Generic methods not associated with specific events.
	 */
	static void log(char const*);
	static void log(uint8_t);
	static void log(unsigned long long int);
	static void logInt(uint32_t); // ??? ambiguous call problems if omit this

	/*
	 * Only using during development, while measuring software.
	 */
	static void logSendLatency(uint32_t);

	/*
	 * Significant events?  Logged remotely?
	 */
	static void logStartSync();
	static void logPauseSync();
	static void logStartFish();
	static void logStopFish();

	static void logMasterDropout();
	static unsigned int logBrownout();

	static void logReceivedInfo(uint8_t);

	static void logResult(char const* label, bool result);
};
