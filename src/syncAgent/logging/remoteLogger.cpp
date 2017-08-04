
#include "remoteLogger.h"

#include <services/mailbox.h>

#include "../slots/infoSlot.h"



namespace {

Mailbox mailbox;

}

void RemoteLogger::log(uint8_t item){
	mailbox.put(item);
}

bool RemoteLogger::trySendingLog(){
	bool result = false;

	if (mailbox.isMail()) {
		InfoSlot::perform(mailbox.fetch());
		result = true;
	}
	return result;
}
