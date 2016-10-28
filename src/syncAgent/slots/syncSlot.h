
#pragma once

#include "../modules/message.h"
#include "slot.h"

class SyncSlot: Slot {

public:
	static void perform();
private:
	static bool dispatchMsgReceived(SyncMessage* msg);

	static void doAbandonMastershipMsg(SyncMessage* msg);
	static void doWorkMsg(SyncMessage* msg);
	static bool doSyncMsg(SyncMessage* msg);	//MasterSync OR MergeSync


	static void doMasterSyncSlot();
	static void doSlaveSyncSlot();
	static bool doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)());
	static void doIdleSlotRemainder();

	static void changeMaster(SyncMessage* msg);
	static bool shouldTransmitSync();
	static void sendMasterSync();
	static void makeCommonMasterSyncMessage();
	static void logWorseSync();
};
