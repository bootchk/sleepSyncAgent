
#pragma once

#include "../types.h"  // SystemID

/*
 * SyncAgent specialized message
 *
 * Two classes:
 * - sync related
 * - work related
 */

// TODO subclass for Sync msg

enum MessageType {
	Sync,		// offset is zero
	MergeSync,	// offset is non-zero
	AbandonMastership,
	Work
};

class Message {
public:
	// provided by wireless stack
	SystemID senderID;

	// Our content of msg (not necessarily from wireless stack)
	MessageType type;
	SyncOffset offset;	// data of sync msg
	SystemID masterID;	// data of sync msg


	void init(MessageType aType, SyncOffset aOffset, SystemID aMasterID) {
		type = aType;
		offset = aOffset;
		masterID = aMasterID;
	}

	bool isOffsetSync() {
		return offset > 0;
	}

	// Dying breath message from master which is power failing.
	void makeAbandonMastership(SystemID aMasterID) {
		type = AbandonMastership;
		offset = 0;
		masterID = aMasterID;
	};

	void makeSync(SystemID aMasterID) {
		type = Sync;
		offset = 0;	// FUTURE, corrections?
		masterID = aMasterID;
	};

	void makeWork() {
		type = Work;
		// TODO content is different
		offset = 0;	// FUTURE, corrections?
		masterID = 0;
	};
};
