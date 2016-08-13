
#pragma once

enum MessageType {
	Sync,
	AbandonMastership,
	Work
};

class Message {
public:
	MessageType type;

	bool isOffsetSync() {return true;}
};
