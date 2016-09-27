

#include "../augment/powerManager.h"
#include "../platform/radio.h"

#include "modules/message.h"
#include "modules/clique.h"
#include "modules/dropoutMonitor.h"
#include "modules/cliqueMerger.h"
#include "modules/role.h"
#include "modules/serializer.h"

/*
 * SyncAgent manages sleep synchronization for wireless network.
 * System is low-power, sleeping much of the time.
 *
 * A task/thread that never returns.
 * Collaborates with WirelessStack task (higher priority) and Work task (lower priority.)
 *
 * SyncAgent turns radio off and on (telling WirelessStack to do so.)
 * While radio is on, SyncAgent waits on events from WirelessStack.
 *
 * A work thread sends work messages through SyncAgent via a queue.
 * SyncAgent delivers received work messages to work thread via a queue.
 *
 * Collaborates with PowerManager.
 * SyncAgent is always cycling through sync periods,
 * but when not enough power, does not keep in sync using WirelessStack.
 *
 * Singleton: all members static, no this.
 * Only use of heap is for messages
 * TODO ???
 */

class SyncAgent {

public:
	static void init( void (*onWorkMsgQueued)() );
	static void loop();
	
	/*
	 * OBS
	 * App calls startSyncing on mcu power on reset POR.
	 * App can not stop synching.
	 * SyncAgent monitors power and tells app onSyncLostCallback.
	 * App continues with mcu in low power, radio not on.
	 * When power is restored, app calls SyncAgent.resume()
	 *
	 * Proper sequence:  POR, startSynching, onSyncingPausedCallback, resumeSyncing, onSyncingPausedCallback, resumeSyncing,...
	 */

	//static void resumeAfterPowerRestored();


private:


// data members
	static bool isSyncingState;
	static uint8_t receiveBuffer[Radio::MaxMsgLength];

	// has-a, all singletons
	static Clique clique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static Role role;
	static SyncMessage outwardSyncMsg;	// Reused, only one message queued at a time // TODO dynamic?
	static WorkMessage workMsg;	// Many may be queued??
	static Serializer serializer;
	static PowerManager powerMgr;



	// Owned by app
	static Radio* radio;
	static void (*onWorkMsgQueuedCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused

// methods

	static void startSyncing();
	static void doSyncPeriod();
	static void dispatchMsgUntil(
			bool (*dispatchQueuedMsg)(),
			OSTime (*func)());

	// start of slots
	static void startSyncSlot();
	// work slot starts without an event
	static void startFishSlot();
	static void startMergeSlot();

	// end of slots
	static void endSyncSlot();
	static void endWorkSlot();
	static void endFishSlot();
	// Merge over without event, as soon as xmit


	// dispatch
	static bool dispatchMsgReceivedInSyncSlot();
	static bool dispatchMsgReceivedInWorkSlot();
	static bool dispatchMsgReceivedInFishSlot();
	// Merge slot only xmits, not receive messages


	// scheduling
	// TODO OBS
	/*
	static void scheduleSyncWake();
	*/
	static void scheduleFishWake();
	static void scheduleMergeWake();
	static void scheduleNextSyncRelatedTask();


	// work
	static void startWorkSlot();
	static void relayWorkToApp(WorkMessage* msg);


	static bool isBetterSync(SyncMessage* msg);
	static void pauseSyncing();
	static void doDyingBreath();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();
	static void xmitAproposWork();

	// msg handlers: messageType x slotType, with omissions
	static void doSyncMsgInSyncSlot(SyncMessage* msg);
	static void doSyncMsgInFishSlot(SyncMessage* msg);
	static void doAbandonMastershipMsgInSyncSlot(SyncMessage* msg);
	static void doWorkMsgInSyncSlot(WorkMessage* msg);
	static void doWorkMsgInWorkSlot(WorkMessage* msg);

	// merge
	static void toMergerRole(SyncMessage* msg);
	static void endMergerRole();

	// abandon
	static void tryAssumeMastership(SyncMessage* msg);

	static void xmitSync(SyncMessage&);
	static void xmitWork(WorkMessage&);
};
