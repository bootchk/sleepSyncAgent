

#include "syncAgent.h"

/*
SyncAgent is a task(thread) that infinite loops.
*/

void SyncAgent::loop(){
	startSyncSlot();
	// radio on
	/*
	do {
		
	} while();
	*/
	//waitForMsgOrTimeout(timeToSyncSlotEnd);
	onSyncSlotEnd();
	// work slot follows sync slot with no delay
	startWorkSlot();
	// radio on
	//waitForMsgOrTimeout(timeToWorkSlotEnd);
	onWorkSlotEnd();
	//sleepTilNextSlot(nextSlotEnd);
	
	
	
}