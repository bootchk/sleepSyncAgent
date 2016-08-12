
#include "clique.h"
#include "powerManager.h"
#include "message.h"
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
			void (*onSyncLostCallback)()
			);
	
	//void setTaskScheduler(void callback());
	static void startSyncing();
	// App can not stop synching.  SyncAgent monitors power and tells app onSyncLostCallback


private:
	static bool isSynching;

	// has-a, all singletons
	static Clique clique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static Role role;

	// uses
	static PowerManager* powerMgr;	// owned by app
	static void (*onSyncLostCallback)();	// callback to app


	// callbacks
	static void onSyncWake();
	static void onFishWake();
	static void onMergeWake();

	static void onMsgReceivedInSyncSlot(Message msg);
	static void onSyncSlotEnd();

	// scheduling
	static void scheduleSyncWake();
	static void scheduleNextSyncRelatedTask();

	static void loseSync();
	static void maintainSyncSlot();
	static void doRoleAproposSyncXmit();

	// msg handlers
	static void doSyncMsgInSyncSlot(Message msg);
	static void doAbandonMastershipMsgInSyncSlot(Message msg);
	static void doWorkMsgInSyncSlot(Message msg);
};
