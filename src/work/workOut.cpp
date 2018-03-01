
#include "workOut.h"


namespace {

	// Owned by calling app, implemented by lib nRF5x
	static Mailbox* workOutMailbox;
}


void WorkOut::init(Mailbox* aMailbox) {
	workOutMailbox = aMailbox;
}


bool WorkOut::isNeedSendWork() {
 return workOutMailbox->isMail();
}

MailContents WorkOut::fetch() {
	return workOutMailbox->fetch();
}
