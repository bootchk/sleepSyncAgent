
#pragma once


enum class SyncSlotKind {
	sendSync,
	sendControlSync,
	sendWorkSync,
	listen
};


class SyncSlotProperty {
public:
	static SyncSlotKind decideKind();
};
