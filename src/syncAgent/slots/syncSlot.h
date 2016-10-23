
#pragma once

#include "../modules/message.h"
#include "../modules/policy/dropoutMonitor.h"

class SyncSlot {
	// owns
	static DropoutMonitor dropoutMonitor;

public:
	static void perform();
private:
	static void startSyncSlot();
	static void endSyncSlot();
	static bool dispatchMsgReceived(SyncMessage* msg);

	static void doAbandonMastershipMsg(SyncMessage* msg);
	static void doWorkMsg(SyncMessage* msg);
	static bool doSyncMsg(SyncMessage* msg);	//MasterSync OR MergeSync


	static void doMasterSyncSlot();
	static void doSlaveSyncSlot();
	static bool doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)());
	static void doIdleSlotRemainder();

	static bool isSyncFromBetterMaster(SyncMessage* msg);
	static void changeMaster(SyncMessage* msg);
	static bool shouldTransmitSync();
	static void sendMasterSync();
	static void makeCommonMasterSyncMessage();
	static void logWorseSync();
	static void checkMasterDroppedOut();
};
