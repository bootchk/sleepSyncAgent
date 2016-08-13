
#pragma once

enum MessageType {
	Sync,		// offset is zero
	MergeSync,	// offset is non-zero
	AbandonMastership,
	Work
};

class Message {
public:
	MessageType type;

	bool isOffsetSync() {return true;}
};
