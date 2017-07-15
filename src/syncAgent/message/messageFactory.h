#pragma once

#include "message.h"

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
		// Fast and loose with casting
		return      (receivedType == (uint8_t) MessageType::MasterSync)
				|| (receivedType == (uint8_t) MessageType::MergeSync)
				|| (receivedType == (uint8_t) MessageType::AbandonMastership)
				|| (receivedType == (uint8_t) MessageType::WorkSync)	// FUTURE Work msg a distinct class of message
				;
	}


	/*
	 * Does this message type have property "carries sync":
	 * - non-null offset?
	 * - non-null MasterID
	 *
	 * AbandonMastership may have null offset
	 */
	static bool carriesSync(MessageType type) {
		bool result;
		switch (type){
		case MessageType::MasterSync:
		case MessageType::MergeSync:
		case MessageType::WorkSync:
			result = true;
			break;
		case MessageType::AbandonMastership:
			result = false;
		}
		return result;
	}

	static SyncMessage* initMasterSyncMessage(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID) {
		message.type = MessageType::MasterSync;
		message.deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// asserts if out of range
		message.masterID = aMasterID;

		// For testing: carry
		message.work = 0;
		return &message;
	}
	static SyncMessage* initMergeSyncMessage(DeltaTime aDeltaToNextSyncPoint,
			SystemID superiorMasterID,
			SystemID inferiorMasterID
			) {
		message.type = MessageType::MergeSync;
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
		message.deltaToNextSyncPoint.set(0);	// throws assertion if out of range
		message.masterID = aMasterID;
		message.work = 0;
		return &message;
	}
};
