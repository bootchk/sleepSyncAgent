
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

#include <nRF5x.h>  // myID()

#include "../logMessage.h"


class SyncSender {
public:

	static void sendMasterSync() {
		log(LogMessage::SendMasterSync);

		/*
		 * Make the common SyncMessage, having:
		 * - type MasterSync
		 * - forwardOffset unsigned delta now to next SyncPoint
		 * - self ID
		 */
		/*
		 * Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
		 * Type DeltaSync ensures that.
		 *
		 * TODO should it also be greater than zero?
		 * Susceptible to breakpoints: If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
		 */
		DeltaTime rawOffset = clique.schedule.deltaNowToNextSyncPoint();

		// TODO robust code: check rawOffset in range now and return if not

		// FUTURE assert we are not xmitting sync past end of syncSlot?
		// i.e. calculations are rapid and sync slot not too short?

		serializer.outwardCommonSyncMsg.makeMasterSync(rawOffset, myID());
		sendPrefabricatedMessage();

		// Uncomment this to experimentally determine send latency.
		//logInt(rawOffset - clique.schedule.deltaNowToNextSyncPoint()); log(":Send latency\n");
	}


	static void sendMergeSync() {
		log(LogMessage::SendMergeSync);

		// cliqueMerger knows how to make global outwardCommonSyncMsg into a MergeSync
		syncAgent.cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg);
		sendPrefabricatedMessage();
	}


	// TODO should calculate offset as late as possible
	static void sendWorkSync() {
		/*
		 * The app sends work OUT only when there is enough power for self to do work,
		 * which is more than is required to send the workSync.
		 * The listener may choose to ignore it if they lack power.
		 * But we must send this workSync because it carries sync.
		 */
		log(LogMessage::SendWorkSync);
		DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
		serializer.outwardCommonSyncMsg.makeWorkSync(
				forwardOffset,
				/*
				 * !!! Crux.  WorkSync identifies the clique Master,
				 * even if self is not the Master I.E. WorkSync could be from a Slave.
				 */
				clique.getMasterID(),
				workOutMailbox->fetch());	// from app, outward
		sendPrefabricatedMessage();
	}


	/*
	 * Convert a prefabricated SyncMessage in global outwardCommonSyncMsg
	 * from object/struct into a byte array, and xmit OTA.
	 */
	static void sendPrefabricatedMessage() {
		// assert sender has created message in outwardCommonSyncMsg
		serializer.serializeOutwardCommonSyncMessage();
		assert(serializer.bufferIsSane());
		radio->transmitStaticSynchronously();
	}
};
