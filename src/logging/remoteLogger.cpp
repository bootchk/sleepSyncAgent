
#include "remoteLogger.h"
#include "logger.h"

// nRF5x
#include <services/mailbox.h>
#include <exceptions/faultHandlers.h>

#include "../slots/info/infoSlot.h"



// define REMOTE_LOGGING to enable, usually defined in build config
#ifdef REMOTE_LOGGING


namespace {

Mailbox infoMailbox;

}


void RemoteLogger::sendInfo(uint8_t item){
	/*
	 * !!! Send it now, performing a slot.
	 */
	InfoSlot::perform(item);
}



bool RemoteLogger::isEnabled() { return true; }

void RemoteLogger::log(uint8_t item){
	infoMailbox.tryPut(item);
}

bool RemoteLogger::trySendingLog(){
	bool result = false;

	if (infoMailbox.isMail()) {
		sendInfo(infoMailbox.fetch());
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

bool RemoteLogger::isEnabled() { return false; }
void RemoteLogger::log(uint8_t item){(void) item; }
// Since not enabled, always not send
bool RemoteLogger::trySendingLog(){ return false; }
void RemoteLogger::sendAnyFaults() {}

#endif

