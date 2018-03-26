
#include "receiver.h"

#include "../messageHandler/messageDispatcher.h"




namespace {

MessageHandler _handler;

void onMsgReceived() {

	// Dispatcher unserializes, filters invalid, and calls handler
	HandlingResult result = MessageDispatcher::dispatch(_handler);

	/*
	 * Not using result.
	 *
	 * Radio may still be receiving, MessageDispatcher can leave it off or on.
	 *
	 * TODO if radio not receiving, might also RadioPrelude::undo()
	 * For now, waste a little energy.
	 */
	(void) result;
}

}	// namespace



void Receiver::startWithHandler(MessageHandler handler) {

	_handler = handler;

	// tell handler to IRQ
	Radio::setMsgReceivedCallback(onMsgReceived);

	// Start task on peripheral
	Ensemble::startReceiving();
}
