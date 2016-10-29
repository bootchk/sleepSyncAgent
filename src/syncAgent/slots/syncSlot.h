
#pragma once

#include "slot.h"

class SyncSlot: Slot {

public:
	static void perform();
private:

	static void doMasterSyncSlot();
	static void doSlaveSyncSlot();
	static bool doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)());
	static void doIdleSlotRemainder();

	static bool shouldTransmitSync();
	static void sendMasterSync();
	static void makeCommonMasterSyncMessage();
};
