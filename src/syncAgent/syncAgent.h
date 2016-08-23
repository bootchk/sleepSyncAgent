

#include "../powerManager.h"

#include "message.h"
#include "clique.h"
#include "dropoutMonitor.h"
#include "cliqueMerger.h"
#include "role.h"


// Singleton: all members static, no this.
class SyncAgent {

public:
	SyncAgent(
			PowerManager* powerMgr,
			void (*onWorkMsgCallback)(WorkMessage* msg)
			);
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
	static bool isSyncing;	// state

	// has-a, all singletons
	static Clique clique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static Role role;
	static SyncMessage outwardSyncMsg;	// Reused, only one message queued at a time // TODO dynamic?
	static WorkMessage workMsg;	// Many may be queued??

	// uses
	static PowerManager* powerMgr;	// owned by app
	static void (*onSyncingPausedCallback)();	// callback to app
	static void (*onWorkMsgCallback)(WorkMessage* msg);	// callback to app

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
};
