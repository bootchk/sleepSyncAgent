
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
		sendInfo(mailbox.fetch());
		result = true;
	}
	return result;
}

void RemoteLogger::sendInfo(uint8_t item){
	InfoSlot::perform(item);
}
