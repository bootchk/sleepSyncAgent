
/*
 * Slot owns MessageHandler registers with Slot.
 * Slot calls dispatchMsgReceived.
 */

#include "../modules/message.h"

typedef bool HandlingResult;

class SyncSlotMessageHandler {
public:
	static HandlingResult dispatchMsgReceived(SyncMessage* msg);

	static HandlingResult doMasterSyncMsg(SyncMessage* msg);
	static HandlingResult doMergeSyncMsg(SyncMessage* msg);
	static HandlingResult doAbandonMastershipMsg(SyncMessage* msg);
	static HandlingResult doWorkMsg(SyncMessage* msg);
};
