
#include <cassert>
#include "cliqueMerger.h"

bool CliqueMerger::active;
int CliqueMerger::notifyCountdown;



void CliqueMerger::adjustBySyncMsg(Message msg) {
	// TODO
	;
}

bool CliqueMerger::isActive() {
	return active;
}

bool CliqueMerger::shouldScheduleMerge() {
	/*
	 * Collision avoidance: choose randomly whether to schedule mergeSlot.
	 *
	 * This implementation may remain in role isMerger a long time (a long random sequence of heads.)
	 * The intent is to minimize scheduled tasks: we don't schedule mergeSlot until the start of period it occurs in.
	 *
	 * Alternative: choose once, a random time in the future, and schedule a mergeSlot in some distant sync period.
	 * But then the task is scheduled for a long time.
	 */
	assert(isActive());	// require
	// TODO CA, random chance
	return true;
}



/* FUTURE
 * // called after sending a merging sync
// xmit finite count of MergeSync
bool CliqueMerger::checkCompletionOfMergerRole() {
	assert(isActive());	// require
	notifyCountdown--;
	bool result = false;
	if ( notifyCountdown <= 0 ){
		result = true;
		active = false;
	}
	return result;
}
*/

void CliqueMerger::setOffsetAndMasterID(){};


int CliqueMerger::timeOfNextMergeWake(){
	// Knows time of mergeSlot i.e. what normally sleeping slot of current period
	// TODO calculate time
	return 1;
}


