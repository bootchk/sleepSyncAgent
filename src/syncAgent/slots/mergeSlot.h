
#pragma once

#include "../modules/policy/mergePolicy.h"

/* MergeSlot of my schedule.
 *
 * Every unit can merge (after fishing catches another clique.)
 *
 * Unique subclass of Slot:
 * - xmit only, no receive.
 * - slot is not full length of other slots, done without event, as soon as xmit.
 * - not aligned with normally sleeping slots
 */

class MergeSlot {

public:
	static MergePolicy mergePolicy;	// visible to SyncAgent
	static void perform();
};
