#pragma once

#include "../modules/message.h"


class Slot {

protected:

	static void prepareRadioToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();
	static void shutdownRadio();

public:
	/*
	 * Default behaviour: return false, don't do anything with msg, and continue looking for messages
	 *
	 * Not pure virtual: subclasses must override for different behaviour.
	 */
	// virtual destructor to avoid warnings
	virtual ~Slot() {};

	virtual bool doMasterSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doMergeSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doAbandonMastershipMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doWorkMsg(SyncMessage* msg) {(void) msg; return false;};
};
