
#include "../provisioning/provisioningPublisher.h"
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
	 * Here we do the latter: the app maintains a In queue, not SyncAgentImp.
	 * The app should do the work later, since now is at imprecise time is a slot.
	 */
	onWorkMsgCallback(work);	// call callback
}
