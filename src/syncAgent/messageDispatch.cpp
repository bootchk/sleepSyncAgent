/*
 * for different slots: sync, work, and fish
 *
 * All have same structure, only differ in semantics of message types handled in slot types.
 *
 * All unqueue a msg, and must free it (or requeue it and then recipient must free it.)
 */

#include <cassert>

#include "../platform/radio.h"
#include "syncAgent.h"

/*
 * result indicates whether desired message was found.
 */
bool SyncAgent::dispatchMsgReceivedInSyncSlot() {
	// TODO while any messages
	bool foundDesiredMessage = false;
	Message* msg = serializer.unserialize(unqueueReceivedMsg());
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			doSyncMsgInSyncSlot((SyncMessage*) msg);
			// Multiple syncs or sync
			foundDesiredMessage = true;
			// TODO discard other queued messages
			freeReceivedMsg((void*) msg);
			break;
		case AbandonMastership:
			doAbandonMastershipMsgInSyncSlot((SyncMessage*) msg);
			freeReceivedMsg((void*) msg);
			break;
		case Work:
			doWorkMsgInSyncSlot((WorkMessage*) msg);
			// !!! msg is moved to work queue, not freed
			break;
		default:
			break;
		}
	}
	// TODO use handle and assert(msgHandle==nullptr);	// callee freed memory and nulled handle, or just nulled handle
	return foundDesiredMessage;
}


bool SyncAgent::dispatchMsgReceivedInWorkSlot(){
	bool foundDesiredMessage = false;
	Message* msg = serializer.unserialize(unqueueReceivedMsg());
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			/* Unusual: Another clique's sync slot at same time as my work slot.
			 * For now, ignore.  Assume fishing will find this other clique, or clocks drift.
			 * Alternative: merge other clique from within former work slot?
			 * doSyncMsgInWorkSlot(msg);
			 */
			freeReceivedMsg((void*) msg);
			break;
		case AbandonMastership:
			/*
			 * Unusual: Another clique's sync slot at same time as my work slot.
			 * For now ignore.  ??? doAbandonMastershipMsgInWorkSlot(msg);
			 */
			freeReceivedMsg((void*) msg);
			break;
		case Work:
			// Usual: work message in sync with my clique.
			doWorkMsgInWorkSlot((WorkMessage*) msg);
			// TODO msg requeued, not freed
			foundDesiredMessage = true;
			break;
		default:
			break;
		}
	}
	return foundDesiredMessage;
}


bool SyncAgent::dispatchMsgReceivedInFishSlot(){
	bool foundDesiredMessage;
	Message* msg = serializer.unserialize(unqueueReceivedMsg());
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			/*
			 * Intended catch: another clique's sync slot.
			 */
			doSyncMsgInFishSlot((SyncMessage*) msg);
			// Self can't handle more than one, or slot is busy with another merge
			turnReceiverOff();
			foundDesiredMessage = true;
			break;
		case AbandonMastership:
			/*
			 * Unintended catch: Another clique's master is abandoning (exhausted power)
			 * For now ignore. Should catch clique again later, after another member assumes mastership.
			 */
			break;
		case Work:
			/*
			 * Unintended catch: Another clique's work slot.
			 * For now ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
			 * Alternative: since work slot follows syncSlot, could calculate syncSlot of catch, and merge it.
			 * Alternative: if work can be done when out of sync, do work.
			 */
			break;
		default:
			break;
		}
		// All msg types freed
		freeReceivedMsg((void*) msg);
	}
	return foundDesiredMessage;
}
