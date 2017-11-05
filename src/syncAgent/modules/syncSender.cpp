
#include <cassert>


#include <radioSoC.h>  // Ensemble, System::ID()

#include "syncSender.h"

#include "../message/message.h"
#include "../message/serializer.h"
#include "../message/messageFactory.h"

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

#ifndef TEST_LATENCY
	// Don't log if testing latency, logging affects measurement
	Logger::logSend(msgPtr);
#endif
	}


}  // namespace


void SyncSender::sendMasterSync() {
	/*
	 * Make MasterSyncMessage, having:
	 * - type MasterSync
	 * - forwardOffset unsigned delta now to next SyncPoint
	 * - self ID
	 *
	 * Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
	 * Type DeltaSync ensures that.
	 */
	DeltaTime rawOffset = clique.schedule.deltaNowToNextSyncPoint();

	/*
	 * Sender specific send latency
	 * The offset will be correct to receiver at the time receiver receives last bit.
	 *
	 * Susceptible to breakpoints: If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
	 * Here we use clampedSubtraction to insure clock subtraction not yield large number.
	 * An alternative design it discussed below: perform simple subtraction and just not send the message
	 * if the result is greater than one SyncPeriodDuration from now.
	 * Again, this situation usually results from debugging.
	 */
	DeltaTime sendLatencyAdjustedOffset = TimeMath::clampedSubtraction(rawOffset, PhysicalParameters::SendLatency);

	// XXX robust code: check sendLatencyAdjustedOffset in range now and return if not
	// XXX rawOffset greater than zero except when breakpointed

	// XXX assert we are not xmitting sync past end of syncSlot?
	// i.e. calculations are rapid and sync slot not too short?

	/*
	 * Time to create message is component of SendLatency.
	 */
	SyncMessage* msgPtr = MessageFactory::initMasterSyncMessage(sendLatencyAdjustedOffset, System::ID());
	sendMessage(msgPtr);

#ifdef TEST_LATENCY
	Logger::logSendLatency(rawOffset - clique.schedule.deltaNowToNextSyncPoint());
#endif
}


void SyncSender::sendMergeSync() {
	// cliqueMerger knows how to create SyncMessage of type MergeSync
	SyncMessage* msgPtr = CliqueMerger::makeMergeSync();
	sendMessage(msgPtr);
}



void SyncSender::sendWorkSync(WorkPayload work) {
	/*
	 * The app sends work OUT only when there is enough power for self to do work,
	 * which is more than is required to send the workSync.
	 * The listener may choose to ignore it if they lack power.
	 * But we must send this workSync because it carries sync.
	 */

	// Temp suppress warning not using work while debugging
	(void) work;


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

void SyncSender::sendAbandonMastership() {
	assert( clique.isSelfMaster);	// Only master can abandon
	SyncMessage* msgPtr = MessageFactory::initAbandonMastershipMessage( clique.getMasterID() );
	sendMessage(msgPtr);
}

void SyncSender::sendInfo(WorkPayload work) {
	SyncMessage* msgPtr = MessageFactory::initInfoMessage( clique.getMasterID(), work );
	sendMessage(msgPtr);
}

