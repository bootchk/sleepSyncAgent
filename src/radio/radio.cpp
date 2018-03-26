
#include "radio.h"

void Radio2::startReceivingWithHandler(MessageHandler) {
	// tell handler to IRQ

	// Start task on peripheral
	Ensemble::startReceiving();
}
