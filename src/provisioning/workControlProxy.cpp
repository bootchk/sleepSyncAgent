
#include "workControlProxy.h"

#include "../network/intraCliqueManager.h"
#include "../syncAgent/syncAgent.h"

#include "../globals.h"
#include "../clique/clique.h"



namespace {

WorkControlCallback workTimeControlCallback = nullptr;
WorkControlCallback workCycleControlCallback = nullptr;


bool isMsgForSelfAsMaster(SyncMessage* msg) {
	return ( clique.isMsgFromMyClique(msg->masterID)
			and SyncAgent::isSelfMaster()
	);
}

}	// namespace




void WorkControlProxy::setWorkTimeControlCallback(WorkControlCallback aCallback) {
	workTimeControlCallback = aCallback;
}

void WorkControlProxy::setWorkCycleControlCallback(WorkControlCallback aCallback) {
	workCycleControlCallback = aCallback;
}

WorkControlCallback WorkControlProxy::getWorkTimeControlCallback() { return workTimeControlCallback; }
WorkControlCallback WorkControlProxy::getWorkCycleControlCallback() { return workCycleControlCallback; }






void WorkControlProxy::handleWorkTimeMessage(SyncMessage* msg) {
	// TODO is network provisioning checking for masterID?
	if ( isMsgForSelfAsMaster(msg) ) {
		/*
		 * Self owns master DistributedWorkClock.
		 * Sync it and soon will start distributing it via WorkSync.
		 */
		// unsigned char silently coerced to unsigned int
		workTimeControlCallback(msg->work);
	}
}
void WorkControlProxy::handleWorkCycleMessage(SyncMessage* msg) {
	if ( isMsgForSelfAsMaster(msg) ) {
		workCycleControlCallback(msg->work);
	}
}
