

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
	// marking start of slots
	static void onSyncWake();
	static void onFishWake();
	static void onMergeWake();
	// marking end of slots
	static void onSyncSlotEnd();
	static void onWorkSlotEnd();
	static void onFishSlotEnd();
	// Merge slots over as soon as xmit

	// callback for external event (varied time within slot)
	static void onMsgReceivedInSyncSlot(Message msg);
	static void onMsgReceivedInWorkSlot(Message msg);
	static void onMsgReceivedInFishSlot(Message msg);


	// scheduling
	static void scheduleSyncWake();
	static void scheduleFishWake();
	static void scheduleMergeWake();
	static void scheduleNextSyncRelatedTask();

	// misc
	static void startWorkSlot();

	static bool isBetterSync(Message msg);
	static void loseSync();
	static void maintainSyncSlot();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();
	static void xmitAproposWork();

	// msg handlers
	static void doSyncMsgInSyncSlot(Message msg);
	static void doAbandonMastershipMsgInSyncSlot(Message msg);
	static void doWorkMsgInSyncSlot(Message msg);

	static void doWorkMsgInWorkSlot(Message msg);

	static void doSyncMsgInFishSlot(Message msg);

	// merging
	static void toMergerRole(Message msg);
	static void completeMergerRole();
};
