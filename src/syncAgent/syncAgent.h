

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
			void (*onSyncingPausedCallback)(),
			void (*onWorkMsgCallback)(SyncMessage msg)
			);
	
	/*
	 * App calls startSyncing on mcu power on reset POR.
	 * App can not stop synching.
	 * SyncAgent monitors power and tells app onSyncLostCallback.
	 * App continues with mcu in low power, radio not on.
	 * When power is restored, app calls SyncAgent.resume()
	 *
	 * Proper sequence:  POR, startSynching, onSyncingPausedCallback, resumeSyncing, onSyncingPausedCallback, resumeSyncing,...
	 */
	static void startSyncing();
	static void resumeAfterPowerRestored();

private:	// data members
	static bool isPaused;	// state

	// has-a, all singletons
	static Clique clique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static Role role;
	static SyncMessage msg;	// Reused, only one message queued at a time
	static WorkMessage workMsg;	// Many may be queued??

	// uses
	static PowerManager* powerMgr;	// owned by app
	static void (*onSyncingPausedCallback)();	// callback to app
	static void (*onWorkMsgCallback)(SyncMessage msg);	// callback to app

private: // methods

	// callbacks for scheduled tasks

	// start of slots
	static void onSyncWake();
	// work slot starts without an event
	static void onFishWake();
	static void onMergeWake();

	// end of slots
	static void onSyncSlotEnd();
	static void onWorkSlotEnd();
	static void onFishSlotEnd();
	// Merge slots over without event, as soon as xmit

	// callback for external event (varied time within slot)
	static void onMsgReceivedInSyncSlot(SyncMessage msg);
	static void onMsgReceivedInWorkSlot(SyncMessage msg);
	static void onMsgReceivedInFishSlot(SyncMessage msg);
	// Merge slot only xmits


	// scheduling
	static void scheduleSyncWake();
	static void scheduleFishWake();
	static void scheduleMergeWake();
	static void scheduleNextSyncRelatedTask();

	// work
	static void startWorkSlot();
	static void relayWorkToApp(SyncMessage msg);


	static bool isBetterSync(SyncMessage msg);
	static void pauseSyncing();
	static void doDyingBreath();
	static void doSyncSlot();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();
	static void xmitAproposWork();

	// msg handlers: messageType x slotType, with omissions
	static void doSyncMsgInSyncSlot(SyncMessage msg);
	static void doSyncMsgInFishSlot(SyncMessage msg);
	static void doAbandonMastershipMsgInSyncSlot(SyncMessage msg);
	static void doWorkMsgInSyncSlot(SyncMessage msg);
	static void doWorkMsgInWorkSlot(SyncMessage msg);

	// merge
	static void toMergerRole(SyncMessage msg);
	static void endMergerRole();

	// abandon
	static void tryAssumeMastership(SyncMessage msg);
};
