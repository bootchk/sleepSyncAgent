
#include "remoteLogger.h"
#include "logger.h"

// nRF5x
#include <services/mailbox.h>
#include <exceptions/faultHandlers.h>

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


void RemoteLogger::sendAnyFaults() {
	if (wasHardFault() or wasAssertionFault() ) {
		sendInfo(HardOrAssertionFault);
	}
}
