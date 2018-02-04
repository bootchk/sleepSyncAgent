
#include "syncAgent.h"


/*
 * Upstream communication from syncAgent to app and other observers.
 */


void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();

ProvisionCallback SyncAgent::onProvisionedCallback;





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


void SyncAgent::subscribeProvisioning(ProvisionCallback aCallback) {
	onProvisionedCallback = aCallback;
}



void SyncAgent::notifyProvisionObservers(uint32_t provisionedValue, int8_t rssi) {
	// TODO filter by rssi
	// Don't callback if provisioner was virtually out of range.

	onProvisionedCallback(provisionedValue);
}
