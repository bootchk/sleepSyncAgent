#pragma once

#include "message.h"

#include "../syncAgent.h"	// Testing

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
		case MessageType::MergeSync:
		case MessageType::AbandonMastership:
		case MessageType::WorkSync:
			result = true;
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
	static bool carriesSync(MessageType type) {
		bool result;
		switch (type){
		case MessageType::MasterSync:
		case MessageType::MergeSync:
		case MessageType::WorkSync:
			result = true;
			break;
		case MessageType::AbandonMastership:
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
		message.deltaToNextSyncPoint.set(0);
		message.masterID = aMasterID;
		message.work = 0;
		return &message;
	}
	static SyncMessage* initInfoMessage(SystemID aMasterID, WorkPayload workPayload) {
		message.type = MessageType::Info;
		message.deltaToNextSyncPoint.set(0);
		message.masterID = aMasterID;
		message.work = workPayload;
		return &message;
	}
};
