
#pragma once

class MergeSlot {

public:
	static MergePolicy mergePolicy;	// visible to SyncAgent
	static void perform();
private:
	static void sendMergeSync();
};
