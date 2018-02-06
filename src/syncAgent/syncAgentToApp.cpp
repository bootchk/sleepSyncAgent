
#include "syncAgent.h"
#include "provisioningPublisher.h"


/*
 * Upstream communication from syncAgent to app and other observers.
 */


void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();



void SyncAgent::relayHeardWorkToApp(WorkPayload work) {
	/*
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 *
	 * Here we do the latter: the app maintains a In queue, not SyncAgent.
	 * The app should do the work later, since now is at imprecise time is a slot.
	 */
	onWorkMsgCallback(work);	// call callback
}

/*
 * No checking:
 * - that index is not also used internally.
 * - that index is in range
 */
void SyncAgent::subscribeProvisioning(PropertyIndex index, ProvisionCallback aCallback) {
	ProvisioningPublisher::subscribe(index, aCallback); // onProvisionedCallback = aCallback;
}




