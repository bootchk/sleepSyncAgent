#pragma once

/*
 * Knows how to fabricate and transmit/broadcast a Sync msg.
 *
 * Collaborates with Radio and Serializer.
 *
 * Note that the caller checked that there is enough power for radio.
 * The power MIGHT have fallen since then, but it is unlikely
 * since only a little receiving has been done.
 * FUTURE: check power more often and abandon in the middle of a slot
 * or in the middle of a sync period (omit listening in the fishSlot)?
 */


#include <ensemble/ensemble.h>
#include <nRF5x.h>  // myID()


#include "../logMessage.h"

// Network previously defined global

class SyncSender {
public:

	static void sendMasterSync() {
		log(LogMessage::SendMasterSync);

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

		// XXX robust code: check rawOffset in range now and return if not
		// XXX rawOffset greater than zero except when breakpointed

		// XXX assert we are not xmitting sync past end of syncSlot?
		// i.e. calculations are rapid and sync slot not too short?

		MasterSyncMessage* msgPtr = serializer.getMasterSyncMsg();
		msgPtr->init(rawOffset, myID());
		sendMessage(msgPtr);

		// Uncomment this to experimentally determine send latency.
		//logInt(rawOffset - clique.schedule.deltaNowToNextSyncPoint()); log(":Send latency\n");
	}


	static void sendMergeSync() {
		log(LogMessage::SendMergeSync);

		// cliqueMerger knows how to tell serializer to init MergeSync
		syncAgent.cliqueMerger.makeMergeSync();
		MergeSyncMessage* msgPtr = serializer.getMergeSyncMsg();
		sendMessage(msgPtr);
	}


	// XXX should calculate offset as late as possible
	// XXX I assume it is only a few tens of instructions, i.e. less than one tick difference?
	static void sendWorkSync(WorkPayload work) {
		/*
		 * The app sends work OUT only when there is enough power for self to do work,
		 * which is more than is required to send the workSync.
		 * The listener may choose to ignore it if they lack power.
		 * But we must send this workSync because it carries sync.
		 */
		log(LogMessage::SendWorkSync);
		DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
		WorkSyncMessage* msgPtr = serializer.getWorkSyncMsg();
		msgPtr->init(
				forwardOffset,
				/*
				 * !!! Crux.  WorkSync identifies the clique Master,
				 * even if self is not the Master I.E. WorkSync could be from a Slave.
				 */
				clique.getMasterID(),
				work);	// from app, outward
		sendMessage(msgPtr);
	}

	static void sendAbandonMastership() {
		log(LogMessage::SendAbandonMastership);
		AbandonMastershipMessage* msgPtr = serializer.getAbandonMastershipMsg();
		assert( clique.isSelfMaster);	// Only master can abandon
		msgPtr->init( clique.getMasterID() );
		sendMessage(msgPtr);
	}


	/*
	 * Convert SyncMessage object into a byte array, and xmit OTA.
	 */
	static void sendMessage(SyncMessage* msgPtr) {
		// assert caller has initialized *msgPtr
		serializer.serializeSyncMessageIntoRadioBuffer(msgPtr);
		assert(serializer.bufferIsSane());

		Ensemble::transmitStaticSynchronously();
	}

};
