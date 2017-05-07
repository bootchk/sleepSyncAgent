
#include "workManager.h"
#include "../globals.h"  // mailbox


namespace {
	static bool heardWork = false;
}


void WorkManager::resetState() {
	heardWork = false;
}

void WorkManager::hearWork() {
	heardWork = true;
}

bool WorkManager::isHeardWork() {
	return heardWork;
}

bool WorkManager::isNeedSendWork() {
 return workOutMailbox->isMail();
}
