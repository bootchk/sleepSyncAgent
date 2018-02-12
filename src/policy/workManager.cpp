
#include "workManager.h"


namespace {
	static bool heardWork = false;

	// Owned by calling app, implemented by lib nRF5x
	static Mailbox* workOutMailbox;
}


void WorkManager::init(Mailbox* aMailbox) {
	workOutMailbox = aMailbox;
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

MailContents WorkManager::fetch() {
	return workOutMailbox->fetch();
}
