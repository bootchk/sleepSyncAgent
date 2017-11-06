#pragma once

#include "../message/message.h"


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
 * We don't throw away information: HandlingResult carries info: what message type was heard, if any.
 */
enum class HandlingResult {
	/*
	 * Means:
	 * - Did not hear intended message
	 * - OR heard intended message but can still hear more
	 *
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
	StopListeningHeardMergeSync,

	/*
	 * Timed out without hearing any messages
	 */
	TimedOut

};



/*
 * Pointer to function taking pointer to SyncMessage and returning HandlingResult.
 */
typedef HandlingResult (*MessageHandler)(SyncMessage* msg);



/*
 * Identical interface, different implementations, but static.
 */
class SyncSlotMessageHandler{
public:
	static HandlingResult handle(SyncMessage* msg);
private:
	static HandlingResult handleMasterSyncMessage(SyncMessage* msg) ;
	static HandlingResult handleMergeSyncMessage(SyncMessage* msg);
	static HandlingResult handleWorkSyncMessage(SyncMessage* msg);
	static HandlingResult handleAbandonMastershipMessage(SyncMessage* msg);
	static HandlingResult handleInfoMessage(SyncMessage* msg);
	static HandlingResult handleControlMessage(SyncMessage* msg);
};


class FishSlotMessageHandler {
public:
	static HandlingResult handle(SyncMessage* msg);
private:
	static HandlingResult handleMasterSyncMessage(SyncMessage* msg) ;
	static HandlingResult handleMergeSyncMessage(SyncMessage* msg);
	static HandlingResult handleWorkSyncMessage(SyncMessage* msg);
	static HandlingResult handleAbandonMastershipMessage(SyncMessage* msg);
	static HandlingResult handleInfoMessage(SyncMessage* msg);
	static HandlingResult handleControlMessage(SyncMessage* msg);
};


#ifdef OLD
/*
 * Super class having dispatcher on code in message.
 * I had much trouble using polymorphic dispatch on SyncMessage*
 */


class MessageHandler {
public:
	// Avoid warning about non-virtual destructor but requires stdlib
	// virtual ~MessageHandler() {};

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


class SyncSlotMessageHandler: public MessageHandler {
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
#endif
