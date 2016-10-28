
#include <cassert>

#include "../globals.h"
#include "slot.h"

void Slot::prepareRadioToTransmitOrReceive() {
	if (!radio->isPowerOn()) {
			radio->powerOnAndConfigure();
			radio->configureXmitPower(8);
		}
	assert(radio->isPowerOn());
	assert(radio->isDisabledState());	// not is receiving
}

void Slot::startReceiving() {
	prepareRadioToTransmitOrReceive();
	syncSleeper.clearReasonForWake();
	radio->receiveStatic();
	assert(!radio->isDisabledState());	// is receiving
}

void Slot::stopReceiving() {
	if (radio->isPowerOn()) {
		radio->stopReceive();
	}
	assert(radio->isDisabledState());	// not is receiving
}

void Slot::shutdownRadio() {
	radio->powerOff();
	assert(!radio->isPowerOn());
}

