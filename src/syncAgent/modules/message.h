
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

	// Usual sync from a master.  Offset is zero FUTURE or small
	void makeSync(SystemID aMasterID) { init(Sync, 0, aMasterID); }

	// sync from master or slave requesting change of sync time.  Offset is large
	void makeMergeSync(SyncOffset aOffset, SystemID aMasterID) { init(Sync, aOffset, aMasterID); }

};

// Messages used by app, relayed by SyncAgent
class WorkMessage : Message {
public:
	// FUTURE: more content.  For testing, no content
	void make() {
		type = Work;
	};
};

