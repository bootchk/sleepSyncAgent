#pragma once

#include "../message/message.h"


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
	static constexpr const char* FishedMergeSync = "Fish Merge\n";
	static constexpr const char* FishedAbandonMastershipSync = "Fish Abandon\n";

	static constexpr const char* MergeMy = "\nMergeMy";
	static constexpr const char* MergeOther = "\nMergeOther";

	static constexpr const char* SendNone = "Listen\n";

	// Role change
	static constexpr const char* ToFisher = "Role fisher\n";
	static constexpr const char* ToMerger = "Role merger\n";

	// Message statistics
	static constexpr const char* Garbled = "Msg type or offset garbled\n";
	static constexpr const char* CRC = "Msg CRC\n";



	static void logStartSyncPeriod(LongTime now);


	// Functions write to flash memory (when not connected to debugger hw)
	// FUTURE put this in some Handler to see?  But I already know what handlers are called.
	//#include "app_util_platform.h"
	// uint32_t ipsr = __get_IPSR();

	// Exhausted power while executing slot sequence
	static void logNoPowerToFish();
	static void logNoPowerToStartSyncSlot();
	static void logNoPowerForHalfSyncSlot();
	static void logOverslept();

	// Only a timer running but it was not the reason for wake.
	static void logUnexpectedWakeReason();

	static void logUnexpectedMsg();
	static void logUnexpectedWakeWhileListening();
	static void logUnexpectedEventWhileListening();

	static void logPauseSync();


	static void logReceivedMsg(SyncMessage* msg);
	static void logSend(SyncMessage* msg);
	static void logMsgDetail(SyncMessage* msg);

	// A msg sent or received, log PeriodTime
	static void logMsgTime();

	static void logInferiorCliqueSyncSlotOfMaster();
	static void logInferiorCliqueSyncSlotOfSlave();

	static void log(char const*);
	static void log(uint8_t);
	static void log(unsigned long long int);
	static void logInt(uint32_t); // TODO ambiguous call problems if omit this

	static void logSendLatency(uint32_t);
};
