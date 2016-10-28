
#pragma once

#include "../../platform/types.h"  // SystemID
#include "../types.h"

/*
 * !!! Message is not a singleton class.
 * Serializer owns static instances.
 * FUTURE: many heap instances, queued.

 * SyncAgent level message, carried as payload in radio messages
 *
 * Current design: WorkMsg is not a separate class,
 * just a SyncMsg having distinct type and variant delta carrying WorkPayload
 *
 * I use 'acceleration' to mean delta of delta: how much a SyncPeriod is changing.
 * Acceleration from a MasterSync is small: deltaToNextSyncPoint is not much different from local time to next SyncPoint
 * Acceleration from a MergeSync is large: deltaToNextSyncPoint is much different from SyncPeriodDuration
 *
 * Subtypes of type Sync, but not separate classes
 * - MergeSync, offset has large acceleration
 * - MasterSync, offset has small acceleration
 * - AbandonMastership, offset is unused
 * - Work, offset is work payload
 *
 * For now, the Work subtype also carries MasterID, and helps achieve sync.
 */


enum MessageType {
	// Subclass SyncMessage
	MasterSync = 17,	// Don't start at 0
	MergeSync = 34,
	AbandonMastership = 68,
	Work = 136
};
// !!!! See hack for mangled Work




// Messages used by SyncAgent, never received by app
class SyncMessage{
public:
	MessageType type;
	DeltaSync deltaToNextSyncPoint;	// forward in time
	SystemID masterID;

	void init(MessageType aType, DeltaSync aDeltaToNextSyncPoint, SystemID aMasterID) {
		type = aType;
		deltaToNextSyncPoint = aDeltaToNextSyncPoint;
		masterID = aMasterID;
	}

	// !!! Work carries enough information to help syncing
	static bool isReceivedTypeASyncType(uint8_t receivedType) {
		return      (receivedType == MasterSync)
				|| (receivedType == MergeSync)
				|| (receivedType == AbandonMastership)
				|| (receivedType == Work)	// FUTURE Work msg a distinct class of message
				;
	}

	/*
	 * Dying breath message from master which is power failing.  deltaToNextSyncPoint is moot.
	 */
	void makeAbandonMastership(SystemID aMasterID) {
		init(AbandonMastership, 0, aMasterID);
	}

	/*
	 * Usual sync from a unit in Master role.
	 * DeltaToNextSyncPoint is typically small.
	 */
	void makeMasterSync(DeltaSync aDeltaToNextSyncPoint, SystemID aMasterID){
		init(MasterSync, aDeltaToNextSyncPoint, aMasterID);
	}

	/*
	 * Sync from unit in Merger role (master or slave) requesting other clique change its sync time.
	 * DeltaToNextSyncPoint is typically but not always large, more than one slot duration.
	 */
	void makeMergeSync(DeltaSync aDeltaToNextSyncPoint, SystemID aMasterID){
		init(MergeSync, aDeltaToNextSyncPoint, aMasterID);
	}

	/*
	 * Work message, also helps to maintain sync.
	 */
	void makeWork(WorkPayload workPayload, SystemID aMasterID){
		// Hack: cast
		init(Work, (DeltaSync) workPayload, aMasterID);
	}

	// See dual: makeWork()
	WorkPayload workPayload() {
		// Hack: cast
		return (WorkPayload) deltaToNextSyncPoint;
	}
};


#ifdef FUTURE
// Superclass
The only thing Work and Sync messages have in common is MessageType field.
class Message {
public:
	// provided by wireless stack??
	// SystemID senderID;

	// Our content of msg (not necessarily from wireless stack)
	MessageType type;
};
#endif

#ifdef OBSOLETE
	bool isOffsetSync() {
		// i.e. used for merge sync
		bool result = deltaToNextSyncPoint > 0;
		return result;
	}
#endif

#ifdef FUTURE
Distinct class for Work????

// Messages used by app, relayed by SyncAgent
class WorkMessage{
public:
	MessageType type;

	// FUTURE: more content.  For testing, no content
	void make() {
		type = Work;
	};
};
#endif


