
/*
 * Knows how to fabricate and transmit/broadcast a Sync msg.
 *
 * Collaborates with Radio and Serializer.
 */

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
		DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
		// FUTURE include correction for latency (on receiver's end)

		// Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
		/*
		 * !!! Soft assertion susceptible to breakpoints.
		 * If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
		 */
		// assert( forwardOffset > 0);

		assert( forwardOffset < clique.schedule.NormalSyncPeriodDuration);

		// FUTURE assert we are not xmitting sync past end of syncSlot?
		// i.e. calculations are rapid and sync slot not too short?

		serializer.outwardCommonSyncMsg.makeMasterSync(forwardOffset, myID());
		sendPrefabricatedMessage();
	}


	static void sendMergeSync() {
		log(LogMessage::SendMergeSync);

		// cliqueMerger knows how to make global outwardCommonSyncMsg into a MergeSync
		syncAgent.cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg);
		sendPrefabricatedMessage();
	}


	static void sendWorkSync() {

		DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
		// FUTURE mailbox is int32, coercing to int8
		serializer.outwardCommonSyncMsg.makeWorkSync(forwardOffset, myID(), (WorkPayload) workOutMailbox->fetch());
		sendPrefabricatedMessage();
	}


	/*
	 * Just the sending aspect, of a prefabricated SyncMessage in global outwardCommonSyncMsg
	 */
	static void sendPrefabricatedMessage() {
		// assert sender has created message in outwardCommonSyncMsg
		serializer.serializeOutwardCommonSyncMessage();
		assert(serializer.bufferIsSane());
		radio->transmitStaticSynchronously();
	}
};
