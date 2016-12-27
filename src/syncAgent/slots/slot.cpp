
#include <cassert>

#include "../globals.h"
#include "slot.h"


void Slot::preamble() {
	radio->hfCrystalClock->startAndSleepUntilRunning();
}

void Slot::postlude() {
	radio->hfCrystalClock->stop();
}

/*
 * If radio not already powered on, make it so.
 */
void Slot::prepareRadioToTransmitOrReceive() {
	if (!radio->isPowerOn()) {
			radio->powerOnAndConfigure();
			// TESTING: lower xmit power 8
			// radio->configureXmitPower(8);
		}
	assert(radio->isPowerOn());
	assert(radio->isDisabledState());	// not is receiving
}

void Slot::startReceiving() {
	// Note radio might already be ready, but this ensure it.
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

