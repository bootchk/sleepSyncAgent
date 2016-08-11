
#include "clique.h"
#include "powerManager.h"
#include "message.h"
#include "dropoutMonitor.h"
#include "cliqueMerger.h"


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

	// uses
	static PowerManager* powerMgr;	// owned by app
	static void (*onSyncLostCallback)();	// callback to app


	// Callbacks
	static void onSyncWake();
	static void onMsgReceivedInSyncSlot(Message msg);
	static void onSyncSlotEnd();

	static void scheduleSyncWake();
	static void loseSync();
	static void maintainSyncSlot();
	static void doRoleAproposSyncXmit();

	// Msg handlers
	static void doSyncMsgInSyncSlot(Message msg);
	static void doAbandonMastershipMsgInSyncSlot(Message msg);
	static void doWorkMsgInSyncSlot(Message msg);
};
