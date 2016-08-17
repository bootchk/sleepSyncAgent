
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
	int offset;	// data of sync msg
	int masterID;	// data of sync msg

	bool isOffsetSync() {
		return offset > 0;
	}
};
