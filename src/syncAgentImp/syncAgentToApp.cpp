
#include "../syncAgentImp/syncAgentImp.h"


/*
 * Upstream communication from syncAgent to app and other observers.
 */


void (*SyncAgentImp::onWorkMsgCallback)(unsigned char);
void (*SyncAgentImp::onSyncPointCallback)();



void SyncAgentImp::relayHeardWorkToApp(unsigned char work) {
	/*
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 *
	 * Here we do the latter: the app might maintain an In queue, not SyncAgentImp.
	 * The app might work later, at a more precise time.
	 *
	 * Now is at imprecise time in a slot.
	 *
	 * !!! "work" is generic.
	 * It might mean "work on demand" or it might maintain a distributed work clock.
	 */
	onWorkMsgCallback(work);	// call callback
}
