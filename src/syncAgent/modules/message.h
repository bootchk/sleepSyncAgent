
#pragma once

#include "../../platform/types.h"  // SystemID
#include "../types.h"

/*
 * SyncAgent specialized message
 *
 * Two classes:
 * - sync related
 * - work related
 *
 * !!! Message is not a static class.
 * Instances can be static (see Serializer ).
 * FUTURE: many instances, queued.
 */


/*
 * application level type of message, carried as payload in radio messages
 *
 * Subtypes of type Sync, but not separate classes
 * - MergeSync, large offset
 * - MasterSync, small offset
 * - AbandonMastership, offset is unused
 * - Work, offset is work type
 *
 * For now, the Work subtype also carries MasterID, and also helps achieve sync
 */


enum MessageType {
	// Subclass SyncMessage
	MasterSync = 1,	// Don't start at 0
	MergeSync,
	AbandonMastership,
	// Subclass WorkMessage
	Work
};
// !!!! See hack for mangled Work

// Superclass
#ifdef FUTURE
The only thing Work and Sync messages have in common is MessageType field.
class Message {
public:
	// provided by wireless stack??
	// SystemID senderID;

	// Our content of msg (not necessarily from wireless stack)
	MessageType type;
};
#endif

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



	// Dying breath message from master which is power failing.  deltaToNextSyncPoint is moot.
	void makeAbandonMastership(SystemID aMasterID) { init(AbandonMastership, 0, aMasterID); }

	/*
	 * Message type is not distinguished MasterSync vs. MergeSync,
	 * so have same implementation.
	 */
	/*
	 * Usual sync from a unit in Master role.
	 * DeltaToNextSyncPoint is typically small.
	 */
	void makeMasterSync(DeltaSync aDeltaToNextSyncPoint,
			SystemID aMasterID)
	{
		init(MasterSync, aDeltaToNextSyncPoint, aMasterID);
	}

	/*
	 * Sync from unit in Merger role (master or slave) requesting other clique change its sync time.
	 * DeltaToNextSyncPoint is typically but not always large, more than one slot duration.
	 */
	void makeMergeSync(DeltaSync aDeltaToNextSyncPoint,
			SystemID aMasterID)
	{
		init(MergeSync, aDeltaToNextSyncPoint, aMasterID);
	}

	static bool isByteASyncType(uint8_t byte) {
		return      (byte == MasterSync)
				|| (byte == MergeSync)
				|| (byte == AbandonMastership);
	}
};

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


