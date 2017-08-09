
#include "remoteLogger.h"
#include "logger.h"

// nRF5x
#include <services/mailbox.h>
#include <exceptions/faultHandlers.h>

#include "../slots/infoSlot.h"



namespace {

Mailbox mailbox;

}


void RemoteLogger::sendInfo(uint8_t item){
	InfoSlot::perform(item);
}

// define REMOTE_LOGGING to enable
#ifdef REMOTE_LOGGING

void RemoteLogger::log(uint8_t item){
	mailbox.tryPut(item);
}

bool RemoteLogger::trySendingLog(){
	bool result = false;

	if (mailbox.isMail()) {
		sendInfo(mailbox.fetch());
		result = true;
	}
	return result;
}

void RemoteLogger::sendAnyFaults() {
	if (wasHardFault() or wasAssertionFault() ) {
		sendInfo(HardOrAssertionFault);
	}
}

#else

void RemoteLogger::log(uint8_t item){(void) item; }
bool RemoteLogger::trySendingLog(){ return false; }
void RemoteLogger::sendAnyFaults() {}

#endif

