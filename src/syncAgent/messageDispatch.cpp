/*
 * for different slots: sync, work, and fish
 *
 * All have same structure, only differ in semantics of message types handled in slot types.
 *
 * All unqueue a msg, and must free it (or requeue it and then recipient must free it.)
 */

#include <cassert>
#include "../platform/radioWrapper.h"
#include "syncAgent.h"

bool SyncAgent::dispatchMsgReceivedInSyncSlot() {
	// TODO while any messages
	bool foundDesiredMessage;
	Message* msg = unqueueMsg();
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			doSyncMsgInSyncSlot((SyncMessage*) msg);
			// Multiple syncs or sync
			foundDesiredMessage = true;
			// TODO discard other queued messages
			freeMsg((void*) msg);
			break;
		case AbandonMastership:
			doAbandonMastershipMsgInSyncSlot((SyncMessage*) msg);
			break;
		case Work:
			doWorkMsgInSyncSlot((WorkMessage*) msg);
			break;
		default:
			break;
		}
	}
	else foundDesiredMessage = false;
	// TODO use handle and assert(msgHandle==nullptr);	// callee freed memory and nulled handle, or just nulled handle
	return foundDesiredMessage;
}


bool SyncAgent::dispatchMsgReceivedInWorkSlot(){
	bool foundDesiredMessage;
	Message* msg = unqueueMsg();
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			/* Unusual: Another clique's sync slot at same time as my work slot.
			 * For now, ignore.  Assume fishing will find this other clique, or clocks drift.
			 * Alternative: merge other clique from within former work slot?
			 * doSyncMsgInWorkSlot(msg);
			 */
			break;
		case AbandonMastership:
			/*
			 * Unusual: Another clique's sync slot at same time as my work slot.
			 * For now ignore.  ??? doAbandonMastershipMsgInWorkSlot(msg);
			 */
			break;
		case Work:
			// Usual: work message in sync with my clique.
			doWorkMsgInWorkSlot((WorkMessage*) msg);
			break;
		default:
			break;
		}
	}
	// TODO
	// assert radio on
	// assert onWorkSlotEnd scheduled
	// sleep
}


bool SyncAgent::dispatchMsgReceivedInFishSlot(){
	bool foundDesiredMessage;
	Message* msg = unqueueMsg();
	if (msg != nullptr) {
		switch(msg->type) {
		case Sync:
			/*
			 * Intended catch: another clique's sync slot.
			 */
			doSyncMsgInFishSlot((SyncMessage*) msg);
			// Self can't handle more than one, or slot is busy with another merge
			turnReceiverOff();
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
	}
	// TODO
}
