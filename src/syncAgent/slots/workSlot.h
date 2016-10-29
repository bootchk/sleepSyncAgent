
#pragma once

#include "slot.h"


/*
 * THE workSlot of my schedule.
 *
 *
 * Receiving MasterSync or MergeSync are unusual: another clique's sync slot at same time as my work slot.
 *
 * Duality: if I hear a Sync in my WorkSlot, then the other clique can hear my work in it's SyncSlot,
 * and could hear my Sync in it's last FishSlot.
 *
 * But we must try to sync it up, since otherwise, none of my clique's fishing slots will find it.
 * IOW, a WorkSlot must act like a FishSlot.
 *
 * Another alternative is to wait for the other clique to drift, but that would take a long time.
 */

class WorkSlot: Slot {
public:
	static void performWork();

private:
	static void performSendingWork();
	static void performReceivingWork();
};
