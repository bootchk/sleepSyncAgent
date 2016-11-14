
#pragma once

#include "slot.h"

class SyncWorkSlot: Slot {

public:
	void perform();

	// override
	bool doMasterSyncMsg(SyncMessage* msg);
	bool doMergeSyncMsg(SyncMessage* msg);
	bool doAbandonMastershipMsg(SyncMessage* msg);
	bool doWorkMsg(SyncMessage* msg);

private:


	void doSendingWorkSyncWorkSlot();
	void doMasterSyncWorkSlot();
	void doSlaveSyncWorkSlot();
	bool doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)());
	// void doIdleSlotRemainder();
};
