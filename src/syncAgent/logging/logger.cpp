
#include "logger.h"

#include <services/logger.h>

#include "flashIndex.h"

#include "../message/message.h"
#include "../modules/schedule.h"


namespace {

/*
 * This is owned instance of interior logger.  Logger class is pure.
 *
 * Implemented using RTT from nRF5x library
 */
RTTLogger logger;

}


void Logger::logStartSyncPeriod(LongTime now) {
	return;
	logger.log(now);
	logger.log("<Sync\n");
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

void Logger::logUnexpectedEventWhileListening() {
	logger.log("\nUnexpected clock event while listening.");
}

void Logger::logUnexpectedWakeWhileListening() {
	logger.log("\nUnexpected wake while listening.");
	CustomFlash::writeZeroAtIndex(UnexpectedWakeWhileListen);
}




void Logger::logReceivedMsg(SyncMessage* msg){
	logMsgTime();
	logger.log("\nRX ");
	logger.log(SyncMessage::representation(msg));
	logger.log("ID:");
	logger.log(msg->masterID);
	logger.log("Off:");
	logger.log(msg->deltaToNextSyncPoint.get());
	logger.log("Wk:");
	logger.log(msg->work);
	logger.log("\n");

}

void Logger::logSend(SyncMessage* msg){
	logger.log("TX ");
	logger.log(SyncMessage::representation(msg));
	logger.log(msg->work);
}

void Logger::logMsgDetail(SyncMessage* msg){
	logger.log("\n ID:");
	logger.log(msg->masterID);
	logger.log(" Off:");
	logger.log(msg->deltaToNextSyncPoint.get());
}

// A msg sent or received, log PeriodTime
void Logger::logMsgTime() {
	logger.log("\nPT:");
	logger.log(Schedule::deltaPastSyncPointToNow());
}

/*
 * Log sync message received in SyncSlot from inferior clique (often WorkSync, sometimes other.)
 * See syncBehaviour.cpp
 */
void Logger::logInferiorCliqueSyncSlotOfMaster() { logger.log("Master heard inferior."); }
void Logger::logInferiorCliqueSyncSlotOfSlave() { logger.log("Slave heard inferior."); }

// Simple pass through
void Logger::init() { logger.init(); }
void Logger::log(const char * text) { logger.log(text); }
void Logger::logInt(uint32_t value) { logger.log(value); }
void Logger::log(uint8_t value) { logger.log(value); }
void Logger::log(unsigned long long value) { logger.log(value); }

void Logger::logSendLatency(uint32_t value) {
	logger.log("SLatency ");
	logger.log(value);
}

void Logger::logReceivedInfo(uint8_t value){
	logger.log("\nInfo ");
	logger.log(value);
}


/*
 * Remote logging using Info messages.
 */

/*
 * Using sniffer: no definitive indication this has happened,
 * except a master stops transmitting sync for a while and others suffer master drop out.
 */
void Logger::logPauseSync() {
	// OLD CustomFlash::writeZeroAtIndex(PauseSync);

	// log it locally

	// queue it for later sending using persistent memory so it survives a soft reset
	// Only my clique can possible hear it.
	// FIXME
	// is ensemble on?

	// send it immediately, but only a sniffer will hear it
}

/*
 * Using sniffer: a slave doesn't transmit.
 * You can tell a slave suffers master dropout when it might resume mastership and resume transmitting.
 */
void Logger::logMasterDropout() {
	logger.log("    MASTER DROP OUT\n");
}

/*
 * Leading \n loses some data???
 */
