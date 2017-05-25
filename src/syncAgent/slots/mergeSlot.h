
#pragma once

#include "../policy/mergePolicy.h"


/* MergeSlot of my schedule.
 *
 * Every unit can merge (after fishing catches another clique.)
 *
 * Not really a slot:
 * - xmit only, no receive.
 * - slot is not full length of other slots, done without event, as soon as xmit.
 * - not aligned with normally sleeping slots
 */

class MergeSlot {
private:
	static void perform();
public:
	static MergePolicy mergePolicy;	// visible to SyncAgent
	static void tryPerform();
};
