
//#include "Schedule.h"
#include "clique.h"
#include "powerManager.h"
#include "syncPolicy.h"


/*
 * Singleton: all members static, no this.
 */
class SyncAgent {


public:
	// Compiler defaults ctor
	SyncAgent(PowerManager* powerMgr);
	
	//void setTaskScheduler(void callback());
	void startSyncing();


private:
	static bool isSynching;

	// has-a
	static SyncPolicy syncPolicy;
	static Clique clique;

	// uses
	static PowerManager* powerMgr;	// owned by app

	//Schedule schedule;

	// Callbacks
	static void onSyncWake();
	static void onMsgReceivedInSyncSlot();
	static void onSyncSlotEnd();

	static void scheduleSyncWake();
	static void loseSync();
	static void maintainSyncSlot();
	static void doRoleAproposSyncXmit();

};
