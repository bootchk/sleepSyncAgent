#pragma once

#include "../modules/message.h"


/*
 * Slot owns MessageHandler.
 * Slot passes MessageHandler.handle to Sleeper.
 *
 * Generically: each Slot type can receive all message types.
 * Specialized by subclass: behaviour: each subclass for Slot type handles messages differently.
 */


/*
 * Whether to continue listening in the slot.
 *
 * We don't throw away information but carry what message type was heard.
 * Note quiet conversions to bool
 */
enum HandlingResult {
	/*
	 * Means:
	 * - Did not hear intended message
	 * - OR heard intended message but can still hear more
	 * - OR timed out without hearing any messages
	 * In other words, heard intended message is false.
	 */
	KeepListening,
	/*
	 * Heard intended message (usually keeping sync)
	 * OR heard unintended message that obviates further listening
	 * These all convert to bool true
	 */
	StopListeningHeardMasterSync,
	StopListeningHeardWorkSync,
	StopListeningHeardMergeSync

};


/*
 * Super class having dispatcher on code in message.
 * I had much trouble attempting to use polymorphic dispatch on SyncMessage*
 */
class MessageHandler {
public:
	virtual ~MessageHandler() {};	// Avoid warning about non-virtual destructor

	// Dispatcher
	// Can't be static
	HandlingResult handle(SyncMessage* msg);

	//virtual HandlingResult handle(SyncMessage* msg) = 0;
	virtual HandlingResult handleMasterSyncMessage(SyncMessage* msg) = 0;
	virtual HandlingResult handleMergeSyncMessage(SyncMessage* msg) = 0;
	virtual HandlingResult handleWorkSyncMessage(SyncMessage* msg) = 0;
	virtual HandlingResult handleAbandonMastershipMessage(SyncMessage* msg) = 0;
};



/*
 * Two implementations for SyncSlot and FishSlot
 */


class SyncSlotMessageHandler : public MessageHandler {
public:
	~SyncSlotMessageHandler() {};

	HandlingResult handleMasterSyncMessage(SyncMessage* msg) ;
	HandlingResult handleMergeSyncMessage(SyncMessage* msg);
	HandlingResult handleWorkSyncMessage(SyncMessage* msg);
	HandlingResult handleAbandonMastershipMessage(SyncMessage* msg);
};


class FishSlotMessageHandler : public MessageHandler {
public:
	~FishSlotMessageHandler() {};

	HandlingResult handleMasterSyncMessage(SyncMessage* msg) ;
	HandlingResult handleMergeSyncMessage(SyncMessage* msg);
	HandlingResult handleWorkSyncMessage(SyncMessage* msg);
	HandlingResult handleAbandonMastershipMessage(SyncMessage* msg);
};