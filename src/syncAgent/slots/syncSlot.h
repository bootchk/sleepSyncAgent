
#pragma once

#include "slot.h"

class SyncSlot: Slot {

public:
	void perform();

	// override
	bool doMasterSyncMsg(SyncMessage* msg);
	bool doMergeSyncMsg(SyncMessage* msg);
	bool doAbandonMastershipMsg(SyncMessage* msg);
	bool doWorkMsg(SyncMessage* msg);

private:

	void doMasterSyncSlot();
	void doSlaveSyncSlot();
	bool doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)());
	void doIdleSlotRemainder();

	static bool shouldTransmitSync();
	static void sendMasterSync();
};
