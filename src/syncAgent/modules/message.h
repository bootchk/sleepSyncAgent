
#pragma once

#include "../../platformHeaders/types.h"  // SystemID
#include "deltaSync.h"


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
 * - WorkSync, work payload is initialized
 *
 * The WorkSync subtype helps achieve sync, but has an extra field "work" that is not used by other subtypes.
 * In an earlier design, the "offset" field was a union, used to carry "work".
 * An alternative design is a distinct WorkMessage class.
 */

/*
 * Encoding for OTA type
 */
enum MessageType {
	// Subclass SyncMessage
	MasterSync = 17,	// Don't start at 0
	MergeSync = 34,
	AbandonMastership = 68,
	WorkSync = 136
};





/*
 *  Messages used by SyncAgent.
 *
 *  Work messages are not a separate, but work field is piggybacked.
 *  Only the work field is received by app.
 *
 *  !!! Note the order and packing of fields in struct is not necessarily order of data in serialized OTA.
 *  The size of the fields is nearly the same as OTA, but e.g. a 24-bit OTA field could be stored in 32-bit field of struct.
 */



class SyncMessage{
public:

	MessageType type;
	DeltaSync deltaToNextSyncPoint;	// forward in time
	SystemID masterID;
	WorkPayload work;	// work always present, not always defined

	// Only to suppress effective C level warnings
	SyncMessage() : type(MasterSync), deltaToNextSyncPoint(), masterID(0), work(0) {}

	// Suppress warnings
    virtual ~SyncMessage() {};

    // Each subclass defines its own signature of init()

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
	 * Should this subclass have:
	 * - non-null offset?
	 * - non-null MasterID
	 *
	 * AbandonMastership may have null offset
	 */
	virtual bool carriesSync() = 0;
};


/*
 * Usual sync from a unit in Master role.
 * DeltaToNextSyncPoint is typically small.
 */
class MasterSyncMessage : public SyncMessage {
public:

	~MasterSyncMessage() {};

	void init(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID) {
		type = MasterSync;
		deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// asserts if out of range
		masterID = aMasterID;
		work = 0;
	}

	virtual bool carriesSync() { return true; }
};



/*
 * Sync from unit in Merger role (master or slave) requesting other clique change its sync time.
 * DeltaToNextSyncPoint is typically but not always large, more than one slot duration.
 */
class MergeSyncMessage : public SyncMessage {
public:
	void init(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID) {
		type = MergeSync;
		deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// throws assertion if out of range
		masterID = aMasterID;
		work = 0;
	}
	~MergeSyncMessage() {};
	virtual bool carriesSync() { return true; }
};


/*
 * Work message, also helps to maintain sync.
 */
class WorkSyncMessage : public SyncMessage {
public:
	void init(DeltaTime aDeltaToNextSyncPoint, SystemID aMasterID, WorkPayload workPayload) {
		type = WorkSync;
		deltaToNextSyncPoint.set(aDeltaToNextSyncPoint);	// throws assertion if out of range
		masterID = aMasterID;
		work = workPayload;
	}
	~WorkSyncMessage() {};
	virtual bool carriesSync() { return true; }
};




/*
 * Dying breath message from master which is power failing.  deltaToNextSyncPoint is moot.
 */
class AbandonMastershipMessage : public SyncMessage {
public:
	void init(SystemID aMasterID) {
			type = AbandonMastership;
			deltaToNextSyncPoint.set(0);	// throws assertion if out of range
			masterID = aMasterID;
			work = 0;
		}
	~AbandonMastershipMessage() {};
	virtual bool carriesSync() { return false; }
};




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


