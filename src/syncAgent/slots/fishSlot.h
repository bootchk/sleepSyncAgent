
#pragma once

#include "slot.h"


class FishSlot: Slot {
public:
	void perform();

	bool doMasterSyncMsg(SyncMessage* msg);
	bool doMergeSyncMsg(SyncMessage* msg);
	bool doWorkMsg(SyncMessage* msg);
	// inherited doAbandonMastershipMsg
};
