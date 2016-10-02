
#pragma once

#include "../../platform/types.h"  // SystemID
#include "../types.h"

/*
 * SyncAgent specialized message
 *
 * Two classes:
 * - sync related
 * - work related
 */


/*
 * application level type of message, carried as payload in radio messages
 *
 * A merge sync is of type Sync but with non-zero offset
 */
enum MessageType {
	// Subclass SyncMessage
	Sync,
	AbandonMastership,
	// Subclass WorkMessage
	Work
};

// Superclass
class Message {
public:
	// provided by wireless stack??
	// SystemID senderID;

	// Our content of msg (not necessarily from wireless stack)
	MessageType type;
};


// Messages used by SyncAgent, never received by app
class SyncMessage : public Message {
public:
	SyncOffset offset;
	SystemID masterID;

	void init(MessageType aType, SyncOffset aOffset, SystemID aMasterID) {
		type = aType;
		offset = aOffset;
		masterID = aMasterID;
	}

	bool isOffsetSync() {
		// i.e. used for merge sync
		bool result = offset > 0;
		return result;
	}

	// Dying breath message from master which is power failing.  Offset is moot.
	void makeAbandonMastership(SystemID aMasterID) { init(AbandonMastership, 0, aMasterID); }

	/*
	 * Message type is not distinguished MasterSync vs. MergeSync,
	 * so have same implementation.
	 */
	/*
	 * Usual sync from a unit in Master role.
	 * Offset is typically small (less than one slot duration.)
	 */
	void makeMasterSync(SyncOffset aOffset,
			SystemID aMasterID)
	{
		init(Sync, aOffset, aMasterID);
	}

	/*
	 * Sync from unit in Merger role (master or slave) requesting other clique change its sync time.
	 * Offset is typically but not always large, more than one slot duration.
	 */
	void makeMergeSync(SyncOffset aOffset,
			SystemID aMasterID)
	{
		init(Sync, aOffset, aMasterID);
	}

};

// Messages used by app, relayed by SyncAgent
class WorkMessage : Message {
public:
	// FUTURE: more content.  For testing, no content
	void make() {
		type = Work;
	};
};

