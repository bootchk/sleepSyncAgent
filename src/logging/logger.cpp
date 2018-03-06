
#include "logger.h"

/*
 * Local and remote interior loggers.
 * Logger uses local RTTLogger and RemoteLogger.
 */
#include <services/logger.h>
#include "remoteLogger.h"

#include "flashIndex.h"

#include "../message/message.h"
#include "../clique/schedule.h"

#include "../globals.h" // clique
#include "../clique/clique.h"



namespace {

/*
 * Owned instance of interior local logger.
 *
 * Implemented using RTT from nRF5x library
 */
RTTLogger localLogger;

}


void Logger::logSystemInfo() {
	localLogger.log(clique.getMasterID());
	localLogger.log("<ID\n");
}


void Logger::logStartSyncPeriod(LongTime now) {
#ifdef FULL_LOGGING
	localLogger.log(now);
	localLogger.log("<Sync\n");
#else
	(void) now;
#endif
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
	localLogger.log("\nUnexpected clock event while listening.");
}

/*
 * All our IRQ's set reason.
 * Maybe a default IRQ ran (without setting reason)
 * or any other unexpected return from WFE.
 */
void Logger::logWakeWithoutIRQSettingReason() {
	localLogger.log("\nWake w/o known IRQ set reason.");
	CustomFlash::writeZeroAtIndex(UnexpectedWakeWhileListen);
}

/*
 * One of our IRQ's was called but found no event flag
 * and set ReasonForWake::Unknown.
 * Not used?
 */
void Logger::logWakeIRQFoundNoEvent() {
	localLogger.log("\nWake IRQ returned reason Unknown.");
}




void Logger::logReceivedMsg(SyncMessage* msg){
	logTicksSinceStartSyncPeriod();
	localLogger.log("\nRX ");
	localLogger.log(SyncMessage::representation(msg));
	localLogger.log(":");
	// shortID
	localLogger.log((uint8_t) msg->masterID);
	//localLogger.log(msg->masterID);
	localLogger.log(":");
	localLogger.log(msg->deltaToNextSyncPoint.get());
	localLogger.log(":");
	localLogger.log(msg->work);
	localLogger.log("\n");

}

void Logger::logSend(SyncMessage* msg){
	localLogger.log("TX ");
	localLogger.log(SyncMessage::representation(msg));
	localLogger.log(msg->work);
}

void Logger::logMsgDetail(SyncMessage* msg){
	localLogger.log("\n ID:");
	localLogger.log(msg->masterID);
	localLogger.log(" Off:");
	localLogger.log(msg->deltaToNextSyncPoint.get());
}

void Logger::logTicksSinceStartSyncPeriod() {
	localLogger.log(" PT:");
	localLogger.log(Schedule::deltaPastSyncPointToNow());
}

/*
 * Log sync message received in SyncSlot from inferior clique (often WorkSync, sometimes other.)
 * See syncBehaviour.cpp
 */
void Logger::logInferiorCliqueSyncSlotOfMaster() { localLogger.log("Master heard inferior."); }
void Logger::logInferiorCliqueSyncSlotOfSlave() { localLogger.log("Slave heard inferior."); }

// Simple pass through
void Logger::init() { localLogger.init(); }
void Logger::log(const char * text) { localLogger.log(text); }
void Logger::logInt(uint32_t value) { localLogger.log(value); }
void Logger::log(uint8_t value) { localLogger.log(value); }
void Logger::log(unsigned long long value) { localLogger.log(value); }

void Logger::logSendLatency(uint32_t value) {
	localLogger.log("SLatency ");
	localLogger.log(value);
}

void Logger::logReceivedInfo(uint8_t value){
	localLogger.log("\nInfo ");
	localLogger.log(value);
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
	localLogger.log("\nPause sync mode. ");
	RemoteLogger::log(RemoteLoggedEvent::StopSync);
}
void Logger::logStartSync() {
	localLogger.log("\nStart sync mode.");
	RemoteLogger::log(RemoteLoggedEvent::StartSync);
}
void Logger::logStartFish() {
	localLogger.log("\nStart fish mode.");
	RemoteLogger::log(RemoteLoggedEvent::StartFish);
}
void Logger::logStopFish() {
	localLogger.log("\nStop fish mode. ");
	RemoteLogger::log(RemoteLoggedEvent::StopFish);
}


void Logger::logIncreaseFish() {
	localLogger.log("\nIncrease fishing duration. ");
	RemoteLogger::log(RemoteLoggedEvent::FishDurationInc);
}
void Logger::logDecreaseFish() {
	localLogger.log("\nDecrease fishing duration. ");
	RemoteLogger::log(RemoteLoggedEvent::FishDurationDec);
}
void Logger::logToMinFish() {
	localLogger.log("\nTo min fishing duration. ");
	RemoteLogger::log(RemoteLoggedEvent::FishDurationMin);
}
//

/*
 * Using sniffer: a slave doesn't transmit.
 * You can tell a slave suffers master dropout when it might resume mastership and resume transmitting.
 */
void Logger::logMasterDropout() {

	localLogger.log("    MASTER DROP OUT\n");	// Local

	RemoteLogger::log(RemoteLoggedEvent::MasterDropOut);
	// TODO symbolic constant
}

/*
 * Log brownout.
 * Used as a callback to BrownoutRecorder.
 * Thus this is called when brownout is in progress.
 * Remote logging does not take much energy, but sending remote might never occur.
 */
unsigned int Logger::logBrownout() {
	RemoteLogger::log(RemoteLoggedEvent::BrownOut);
	return 1;	// dummy value to be recorded to UICR
}


void Logger::logResult(char const* label, bool result) {
	log(label); log(": ");
	if (result)
		log("Y\n");
	else
		log("N\n");
}

/*
 * Leading \n loses some data???
 */
