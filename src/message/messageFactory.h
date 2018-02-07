#pragma once

#include "message.h"

#include "../syncAgent/syncAgent.h"	// Testing

namespace {

// One message instance, reused
SyncMessage message;

}


/*
 * Knows how to construct a message.
 * Understands message types and their properties
 */
class MessageFactory {

public:

	// Get pointer to singleton instance
	static SyncMessage* getMessagePtr(){ return &message; }

	// Does an OTA received byte seem like a MessageType?
	static bool isReceivedTypeASyncType(uint8_t receivedType) {
		// cast it
		MessageType castType = (MessageType) receivedType;

		// check cast was valid
		bool result = false;

		switch(castType) {
		case MessageType::Info:
		case MessageType::MasterSync:
		case MessageType::MasterMergedAway:
		case MessageType::SlaveMergedAway:
		case MessageType::EnticingInferior:
		case MessageType::AbandonMastership:
		case MessageType::WorkSync:

		case MessageType::WorkSetProximity:
		case MessageType::WorkScatterTime:
		case MessageType::ControlNetGranularity:
		case MessageType::ControlScatterClique:
			result = true;
		}

		return result;
	}

	/*
	 * Is message related to merging?
	 */
	// NOT USED?
	static bool doesCarryOffsetToOtherClique(MessageType type) {
		bool result;
		switch (type){
		case MessageType::MasterSync:
		case MessageType::EnticingInferior:
		case MessageType::WorkSync:
			result = true;
			break;
		default:
			result = false;
		}
		return result;
	}


	/*
	 * Does this message type have property "carries sync":
	 * - non-null offset?
	 * - non-null MasterID
	 *
	 * AbandonMastership may have null offset
	 */
	static bool doesCarrySync(MessageType type) {
		bool result;
		switch (type){
		case MessageType::MasterSync:
		case MessageType::WorkSync:
			result = true;
			break;

		// Carry offset, but for DeepFishing
		case MessageType::MasterMergedAway:
		case MessageType::SlaveMergedAway:
		case MessageType::EnticingInferior:

		// Offset field not used?
		case MessageType::AbandonMastership:
		case MessageType::WorkSetProximity:
		case MessageType::WorkScatterTime:
		case MessageType::ControlNetGranularity:
		case MessageType::ControlScatterClique:
		case MessageType::Info:
			result = false;
		}
		return result;
	}

	static SyncMessage* initMasterSyncMessage(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID) {
		message.type = MessageType::MasterSync;
		message.deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// asserts if out of range
		message.masterID = aMasterID;

		// Normally work is empty.  For testing: carry
		message.work = SyncAgent::countMergeSyncHeard;
		return &message;
	}
	static SyncMessage* initMergeSyncMessage(MessageType aType, DeltaTime aDeltaToNextSyncPoint,
			SystemID superiorMasterID,
			SystemID inferiorMasterID
			) {
		message.type = aType;
		message.deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// throws assertion if out of range
		message.masterID = superiorMasterID;

		// Work is normally unused, sent as zero.  For testing: send lower two bytes of inferior
		message.work = (WorkPayload) inferiorMasterID;
		return &message;
	}
	static SyncMessage* initWorkSyncMessage(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID, WorkPayload workPayload) {
		message.type = MessageType::WorkSync;
		message.deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// throws assertion if out of range
		message.masterID = aMasterID;
		message.work = workPayload;
		return &message;
	}
	static SyncMessage* initAbandonMastershipMessage(SystemID aMasterID) {
		message.type = MessageType::AbandonMastership;
		message.deltaToNextSyncPoint.set(0);
		message.masterID = aMasterID;
		message.work = 0;
		return &message;
	}

	/*
	 * Message types that don't carry sync.
	 */
	static SyncMessage* initInfoMessage(SystemID aMasterID, WorkPayload workPayload) {
		initNonSyncMessage(MessageType::Info, aMasterID, workPayload);
		return &message;
	}
/*
	static SyncMessage* initControlSetXmitPowerMessage(SystemID aMasterID, WorkPayload workPayload) {
		initNonSyncMessage(MessageType::ControlSetXmitPower, aMasterID, workPayload);
		return &message;
	}
*/
	static SyncMessage* initControlMessage(
			SystemID aMasterID,
			MessageType aType,
			WorkPayload workPayload)
	{
		// TODO control message is non-sync?
		initNonSyncMessage(aType, aMasterID, workPayload);
		return &message;
	}

	static SyncMessage* initNonSyncMessage(MessageType msgType, SystemID aMasterID, WorkPayload workPayload) {
		message.type = msgType;
		message.deltaToNextSyncPoint.set(0);
		message.masterID = aMasterID;
		message.work = workPayload;
		return &message;
	}


};
