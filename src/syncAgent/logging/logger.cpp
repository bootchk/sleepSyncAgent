
#include "logger.h"

#include "flashIndex.h"

#include "../message/message.h"
#include "../modules/schedule.h"





void Logger::logStartSyncPeriod(LongTime now) {
	return;
	logLongLong(now);
	log("<Sync\n");
}



// Functions write to flash memory (when not connected to debugger hw)
// FUTURE put this in some Handler to see?  But I already know what handlers are called.
//#include "app_util_platform.h"
// uint32_t ipsr = __get_IPSR();

// Exhausted power while executing slot sequence
void Logger::logNoPowerToFish() { CustomFlash::writeZeroAtIndex(NoPowerToFish); }
void Logger::logNoPowerToStartSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToStartSyncSlot); }
void Logger::logNoPowerForHalfSyncSlot() { CustomFlash::writeZeroAtIndex(NoPowerToHalfSyncSlot); }
void Logger::logOverslept() { CustomFlash::writeZeroAtIndex(OverSlept); }

// Only a timer running but it was not the reason for wake.
void Logger::logUnexpectedWakeReason() { CustomFlash::writeZeroAtIndex(UnexpectedWake); }

void Logger::logUnexpectedMsg() { CustomFlash::writeZeroAtIndex(UnexpectedMsg); }
void Logger::logUnexpectedWakeWhileListening() { CustomFlash::writeZeroAtIndex(UnexpectedWakeWhileListen); }

void Logger::logPauseSync() { CustomFlash::writeZeroAtIndex(PauseSync); }


void Logger::logReceivedMsg(SyncMessage* msg){
	log("\nRX ");
	log(SyncMessage::representation(msg));
	logLongLong(msg->masterID);
	logByte(msg->work);

}

void Logger::logSend(SyncMessage* msg){
	log("TX ");
	log(SyncMessage::representation(msg));
	logByte(msg->work);
}

void Logger::logMsgDetail(SyncMessage* msg){
	log("\n ID:");
	logLongLong(msg->masterID);
	log(" Off:");
	logInt(msg->deltaToNextSyncPoint.get());
}

// A msg sent or received, log PeriodTime
void Logger::logMsgTime() {
	log("\nPT:");
	logInt(Schedule::deltaPastSyncPointToNow());
}

/*
 * Log sync message received in SyncSlot from inferior clique (often WorkSync, sometimes other.)
 * See syncBehaviour.cpp
 */
void Logger::logInferiorCliqueSyncSlotOfMaster() { log("Master heard inferior."); }
void Logger::logInferiorCliqueSyncSlotOfSlave() { log("Slave heard inferior."); }


