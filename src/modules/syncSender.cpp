
#include <cassert>


#include <radioSoC.h>  // Ensemble, System::ID()

#include "syncSender.h"

#include "../message/message.h"
#include "../message/serializer.h"
#include "../message/messageFactory.h"

#include "../modules/syncOffset.h"

#include "../globals.h"	// clique
#include "../logging/logger.h"

#include "../network/intraCliqueManager.h"

#include "../syncAgent/syncAgent.h"


/*
 * Offset should be calculated as close to OTA time as possible.
 * If you insert code between calculate offset and transmit
 * you should remeasure latency and adjust latency parameters
 */

namespace {

/*
 * Implementation:
 *   - serialize SyncMessage object into a byte array
 *   - xmit byte array
 */
static void sendMessage(SyncMessage* msgPtr) {
	// assert caller created valid message
	// assert Ensemble startup was done

#ifndef TEST_LATENCY
	// Don't log if testing latency, logging affects measurement
	Logger::logSend(msgPtr);
#endif

	Serializer::serializeSyncMessageIntoRadioBuffer(msgPtr);

	// Use for extreme testing, but affects latency
	// assert(Serializer::bufferIsSane());

	// Takes time: RampupDelay + MsgOverTheAirTimeInTicks
	Ensemble::transmitStaticSynchronously();
	}


}  // namespace




void SyncSender::sendMasterSync() {
	/*
	 * Make MasterSyncMessage, having:
	 * - type MasterSync
	 * - forwardOffset unsigned delta now to next SyncPoint
	 * - self ID
	 */
	/*
	 OLD
	 DeltaTime sendLatencyAdjustedOffset = SyncOffset::calculate();
	 SyncMessage* msgPtr = MessageFactory::initMasterSyncMessage(sendLatencyAdjustedOffset, System::ID());
	 */
	SyncMessage* msgPtr = MessageFactory::initSyncMessage(
			MessageType::MasterSync,
			SyncAgent::countMergeSyncHeard );	// debugging content, not required
	sendMessage(msgPtr);

#ifdef TEST_LATENCY
	Logger::logSendLatency(rawOffset - clique.schedule.deltaNowToNextSyncPoint());
#endif
}


void SyncSender::sendMergeSync() {
	// cliqueMerger knows how to create msg of class MergeSync
	SyncMessage* msgPtr = CliqueMerger::makeMergeMsg();
	sendMessage(msgPtr);
}



/*
	 * The app sends work OUT only when there is enough power for self to do work,
	 * which is more than is required to send the workSync.
	 * The listener may choose to ignore it if they lack power.
	 * But we must send this workSync because it carries sync.
	 */
void SyncSender::sendWorkSync(WorkPayload work) {
	SyncMessage* msgPtr = MessageFactory::initSyncMessage(
				MessageType::WorkSync,
				work );
	sendMessage(msgPtr);
}


#ifdef OLD
	DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
	SyncMessage* msgPtr = MessageFactory::initWorkSyncMessage(
			forwardOffset,
			/*
			 * !!! Crux.  WorkSync identifies the clique Master,
			 * even if self is not the Master I.E. WorkSync from a Slave uses Clique's ID.
			 */
			clique.getMasterID(),
			work);
	/*
	 * Temporarily, sending self ID (short version) as work
	 * In most applications, the last parameter is:
	 * work);	// from app, outward
	 */
	sendMessage(msgPtr);
}
#endif


void SyncSender::sendAbandonMastership() {
	assert( clique.isSelfMaster);	// Only master can abandon
	SyncMessage* msgPtr = MessageFactory::initAbandonMastershipMessage( clique.getMasterID() );
	sendMessage(msgPtr);
}

void SyncSender::sendInfo(WorkPayload work) {
	SyncMessage* msgPtr = MessageFactory::initInfoMessage( clique.getMasterID(), work );
	sendMessage(msgPtr);
}


void SyncSender::sendControlSync() {
	/*
	 * Build message from what IntraCliqueManager tells us
	 */
	SyncMessage* msgPtr = MessageFactory::initSyncMessage(
			IntraCliqueManager::currentMsgType(),
			IntraCliqueManager::currentPayload());
	sendMessage(msgPtr);
}

