
#pragma once



class FishSlot{
public:
	static void perform();
	static bool dispatchMsgReceived(SyncMessage* msg);
	static bool doMasterSyncMsg(SyncMessage* msg);
	static bool doMergeSyncMsg(SyncMessage* msg);
	static bool doAbandonMastershipMsg(SyncMessage* msg) {(void) msg; return false;};
	static bool doWorkMsg(SyncMessage* msg);

};
