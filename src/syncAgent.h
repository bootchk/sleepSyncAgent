
//#include "Schedule.h"
#include "clique.h"
#include "powerManager.h"

/*
 * Singleton: all members static, no this.
 */
class SyncAgent {


public:
	// Compiler defaults ctor
	SyncAgent(PowerManager* powerMgr);
	
	void setTaskScheduler(void callback());
	void startSyncing();
	static void onSyncWake();

private:
	static Clique clique;	// has-a
	static PowerManager* powerMgr;	// uses
	//Schedule schedule;

	static void scheduleSyncWake();
};
