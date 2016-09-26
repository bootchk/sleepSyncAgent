
#include "sleepSyncAgent.h"

#include "augment/powerManager.h"
#include "syncAgent/syncAgent.h"

/*
 * Implementation:
 *
 * Largely a wrapper around SyncAgent.
 *
 * PowerManager is not owned by SyncAgent, but used by it.
 * You could implement a null PowerManager (always returning "yes, power is OK"),
 * if you don't care to manage power.
 *
 * The caller might also want to used PowerManager?
 * It is not clear where it should live.
 */


// Private data members.  Anonymous namespace similar to "static"
namespace
{
PowerManager powerMgr;
SyncAgent syncAgent;
}



SleepSyncAgent::SleepSyncAgent(void (*onWorkMsgQueued)()){
	syncAgent.init(&powerMgr, onWorkMsgQueued);
}


void SleepSyncAgent::loopOnEvents() {
	syncAgent.loop();
}

