

#include "../powerManager.h"

#include "message.h"
#include "clique.h"
#include "dropoutMonitor.h"
#include "cliqueMerger.h"
#include "role.h"


/*
 * Singleton: all members static, no this.
 */
class SyncAgent {


public:
	SyncAgent(
			PowerManager* powerMgr,
			void (*onSyncLostCallback)(),
			void (*onWorkMsgCallback)(Message msg)
			);
	
	//void setTaskScheduler(void callback());
	static void startSyncing();
	// App can not stop synching.  SyncAgent monitors power and tells app onSyncLostCallback


private:	// data members
	static bool isSynching;

	// has-a, all singletons
	static Clique clique;
	static Clique otherClique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static Role role;
	static Message msg;	// Reused, only one message queued at a time

	// uses
	static PowerManager* powerMgr;	// owned by app
	static void (*onSyncLostCallback)();	// callback to app
	static void (*onWorkMsgCallback)(Message msg);	// callback to app

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
	static void onMsgReceivedInSyncSlot(Message msg);
	static void onMsgReceivedInWorkSlot(Message msg);
	static void onMsgReceivedInFishSlot(Message msg);
	// Merge slot only xmits


	// scheduling
	static void scheduleSyncWake();
	static void scheduleFishWake();
	static void scheduleMergeWake();
	static void scheduleNextSyncRelatedTask();

	// work
	static void startWorkSlot();
	static void relayWorkToApp(Message msg);


	static bool isBetterSync(Message msg);
	static void loseSync();
	static void maintainSyncSlot();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();
	static void xmitAproposWork();

	// msg handlers: messageType x slotType, with omissions
	static void doSyncMsgInSyncSlot(Message msg);
	static void doSyncMsgInFishSlot(Message msg);
	static void doAbandonMastershipMsgInSyncSlot(Message msg);
	static void doWorkMsgInSyncSlot(Message msg);
	static void doWorkMsgInWorkSlot(Message msg);

	// merge
	static void toMergerRole(Message msg);
	static void completeMergerRole();

	// abandon
	static void tryAssumeMastership(Message msg);
};
