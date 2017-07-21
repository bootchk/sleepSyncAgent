
#include <cassert>


#include <nRF5x.h>  // Ensemble, myID()

#include "syncSender.h"

#include "../message/message.h"
#include "../message/serializer.h"
#include "../globals.h"	// clique
#include "../logging/logger.h"

#include "../physicalParameters.h"


/*
 * Offset should be calculated as close to OTA time as possible.
 * If you insert code between calculate offset and transmit
 * you should remeasure latency and adjust latency parameters
 */

namespace {

/*
 * Convert SyncMessage object into a byte array, and xmit OTA.
 */
static void sendMessage(SyncMessage* msgPtr) {
	// assert caller created valid message
	// assert Ensemble startup was done

	/*
	 * Time to serialize is a component of SendLatency.
	 */
	Serializer::serializeSyncMessageIntoRadioBuffer(msgPtr);

	// Use for extreme testing, but affects latency
	// assert(Serializer::bufferIsSane());

	// Takes time: RampupDelay + MsgOverTheAirTimeInTicks
	Ensemble::transmitStaticSynchronously();

	/*
	 * No logging above since it would affect send latency.
	 * Caller may measure and log send latency.
	 */
}


}


void SyncSender::sendMasterSync() {
	/*
	 * Make MasterSyncMessage, having:
	 * - type MasterSync
	 * - forwardOffset unsigned delta now to next SyncPoint
	 * - self ID
	 *
	 * Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
	 * Type DeltaSync ensures that.
	 *
	 * Susceptible to breakpoints: If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
	 */
	DeltaTime rawOffset = clique.schedule.deltaNowToNextSyncPoint();

	/*
	 * Sender specific send latency
	 * The offset will be correct to receiver when last bit is received.
	 */
	DeltaTime sendLatencyAdjustedOffset = rawOffset - PhysicalParameters::SendLatency;

	// XXX robust code: check rawOffset in range now and return if not
	// XXX rawOffset greater than zero except when breakpointed

	// XXX assert we are not xmitting sync past end of syncSlot?
	// i.e. calculations are rapid and sync slot not too short?

	/*
	 * Time to create message is component of SendLatency.
	 */
	SyncMessage* msgPtr = MessageFactory::initMasterSyncMessage(sendLatencyAdjustedOffset, myID());
	sendMessage(msgPtr);

	Logger::logSendLatency(rawOffset - clique.schedule.deltaNowToNextSyncPoint());
	// Log message after sendLatency
	Logger::logSend(msgPtr);
}


void SyncSender::sendMergeSync() {
	// cliqueMerger knows how to create SyncMessage of type MergeSync
	SyncMessage* msgPtr = syncAgent.cliqueMerger.makeMergeSync();
	sendMessage(msgPtr);
}



void SyncSender::sendWorkSync(WorkPayload work) {
	/*
	 * The app sends work OUT only when there is enough power for self to do work,
	 * which is more than is required to send the workSync.
	 * The listener may choose to ignore it if they lack power.
	 * But we must send this workSync because it carries sync.
	 */
	DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
	SyncMessage* msgPtr = MessageFactory::initWorkSyncMessage(
			forwardOffset,
			/*
			 * !!! Crux.  WorkSync identifies the clique Master,
			 * even if self is not the Master I.E. WorkSync from a Slave uses Clique's ID.
			 */
			clique.getMasterID(),
			myShortID());	// temp debugging
	/*
	 * Temporarily, sending self ID (short version) as work
	 * In most applications, the last parameter is:
	 * work);	// from app, outward
	 */
	sendMessage(msgPtr);
}

void SyncSender::sendAbandonMastership() {
	assert( clique.isSelfMaster);	// Only master can abandon
	SyncMessage* msgPtr = MessageFactory::initAbandonMastershipMessage( clique.getMasterID() );
	sendMessage(msgPtr);
}

