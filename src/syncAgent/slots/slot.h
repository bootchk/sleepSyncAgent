#pragma once


class Slot {

protected:

	static void prepareRadioToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();
	static void shutdownRadio();

public:
	// Default behaviour: return false, don't do anything with msg, and continue looking for messages
	virtual bool doMasterSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doMergeSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doAbandonMastershipMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doWorkMsg(SyncMessage* msg) {(void) msg; return false;};
};
