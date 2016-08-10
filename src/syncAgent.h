
//#include "Schedule.h"
#include "clique.h"

/*
 * Singleton: all members static, no this.
 */
class SyncAgent {


public:
	// Compiler defaults ctor
	//SyncAgent();
	
	void setTaskScheduler(void callback());
	void startSyncing();
	static void onSyncWake();

private:
	static Clique clique;
	//Schedule schedule;

	static void scheduleSyncWake();
};
