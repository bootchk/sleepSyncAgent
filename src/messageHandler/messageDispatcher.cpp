

#include "messageDispatcher.h"


/*
 * Filter invalid messages.
 * Return result of dispatching valid messages.
 */
HandlingResult MessageDispatcher::dispatch( MessageHandler msgHandler) { // Slot has handlers per message type
	// FUTURE while any received messages queued

	HandlingResult handlingResult = HandlingResult::KeepListening;

	// Nested checks: physical layer CRC, then transport layer MessageType
	if (Ensemble::isPacketCRCValid()) {
		SyncMessage* msg = Serializer::unserialize();
		if (msg != nullptr) {
			// assert msg->type valid

			handlingResult = msgHandler(msg);
			if (handlingResult!=HandlingResult::KeepListening) {
				// Remainder of duration radio not used (low power) but HFXO is still on.
				// TODO this assertion should be at beginning of routine
				assert(!Ensemble::isRadioInUse());

				// continuation is sleep
				// assert since radio not in use, next reason for wake can only be timeout and will then exit loop
			}
			else {
				/*
				 * Dispatched message was not of desired type (but we could have done work, or other state changes.)
				 * restart receive, remain in loop, sleep until next message
				 */
				Ensemble::startReceiving();
				// continuation is sleep with radio on
			}
			// assert msg queue is empty (since we received and didn't restart receiver)
		}
		else {
			// Ignore garbled type or offset
			Logger::log(Logger::Garbled);
			// continuation is wait for another message
			Ensemble::startReceiving();
		}

		// No memory managment for messages
	}
	else {
		/* Ignore CRC invalid packet except to log it.
		 *
		 * Note CRCSTATUS register remains showing invalid until another message is received.
		 */
		Logger::log(Logger::CRC);
		// continuation is wait for another message
		Ensemble::startReceiving();

	}

	/*
	 * result "keep listening" implies we restarted the radio
	 * handlingResult==KeepListening=>Ensemble::isRadioInUse
	 * Logical implication P=>Q is equivalent to not P OR Q
	 * handlingResult!=KeepListening OR ensemble::isRadioInUse
	 * TODO add those assertions, for now, there is a downstream assertion
	 */
	return handlingResult;
}
