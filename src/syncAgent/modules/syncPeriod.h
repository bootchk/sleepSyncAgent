
#pragma once


#include "../slots/syncSlot.h"
#include "../slots/workSlot.h"
#include "../slots/fishSlot.h"
#include "../slots/mergeSlot.h"


/*
 * Sequence of Slots.
 * Slots vary according to state of synchronization.
 */

// Abstract Base Class
class SyncPeriod {
private:
	// Owns Slot instances
public:
	/*
	 * One cycle executing the sequence.
	 * Defines slot order and how state determines which slots executed.
	 */
	static void doSlotSequence();
};


class SimpleSyncPeriod : SyncPeriod {

public:
	static void doSlotSequence();
};
