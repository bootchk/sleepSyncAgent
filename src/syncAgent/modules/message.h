
#pragma once

#include "../../platformHeaders/types.h"  // SystemID
#include "../deltaSync.h"


/*
 * !!! Message is not a singleton class.
 * Serializer owns static instances.
 * FUTURE: many heap instances, queued.

 * SyncAgent level message, carried as payload in radio messages
 *
 * Current design: WorkMsg is not a separate class,
 * just a SyncMsg having distinct type and carrying an initialized WorkPayload
 *
 * I use 'acceleration' to mean delta of delta: how much a SyncPeriod is changing.
 * Acceleration from a MasterSync is small: deltaToNextSyncPoint is not much different from local time to next SyncPoint
 * Acceleration from a MergeSync is large: deltaToNextSyncPoint is much different from SyncPeriodDuration
 *
 * Subtypes of type Sync, but not separate classes
 * - MergeSync, offset has large acceleration
 * - MasterSync, offset has small acceleration
 * - AbandonMastership, offset is unused
 * - Work, work payload is initialized
 *
 * The Work subtype is a superset (having an extra field), helps achieve sync.
 */


enum MessageType {
	// Subclass SyncMessage
	MasterSync = 17,	// Don't start at 0
	MergeSync = 34,
	AbandonMastership = 68,
	WorkSync = 136
};
// !!!! See hack for mangled Work




/*
 *  Messages used by SyncAgent.
 *
 *  In this design, work messages are not separate, but work field is piggybacked.
 *  Only the work field is received by app.
 *
 *  !!! Note the order of fields in struct is not necessarily order of data in serialized OTA.
 */
class SyncMessage{
public:
	MessageType type;
	DeltaSync deltaToNextSyncPoint;	// forward in time
	SystemID masterID;
	WorkPayload work;	// work always present, not always defined

	void init(MessageType aType, DeltaSync aDeltaToNextSyncPoint, SystemID aMasterID) {
		type = aType;
		deltaToNextSyncPoint = aDeltaToNextSyncPoint;
		masterID = aMasterID;
		// !!! omits work
	}


	// Class method

	// Does an OTA received byte seem like a MessageType?
	static bool isReceivedTypeASyncType(uint8_t receivedType) {
		return      (receivedType == MasterSync)
				|| (receivedType == MergeSync)
				|| (receivedType == AbandonMastership)
				|| (receivedType == WorkSync)	// FUTURE Work msg a distinct class of message
				;
	}


	/*
	 * Should this kind of SyncMsg have:
	 * - non-null offset?
	 * - non-null MasterID
	 *
	 * AbandonMastership may have null offset
	 */
	static bool carriesSync(MessageType type) {
		return type == MasterSync
				|| type == MergeSync
				|| type == WorkSync;
	}

	/*
	 * Dying breath message from master which is power failing.  deltaToNextSyncPoint is moot.
	 */
	void makeAbandonMastership(SystemID aMasterID) {
		init(AbandonMastership, (DeltaSync) 0, aMasterID);
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
	void makeWorkSync(DeltaSync aDeltaToNextSyncPoint, SystemID aMasterID, WorkPayload workPayload){
		init(WorkSync, aDeltaToNextSyncPoint, aMasterID);
		work = workPayload;
	}

	// See dual: makeWork()
	WorkPayload workPayload() {
		return work;
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


