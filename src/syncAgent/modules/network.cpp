
#include <cassert>

#include "../globals.h"
#include "network.h"

//  TODO rename start and stop peripherals needed by radio

void Network::preamble() {
	radio->hfCrystalClock->startAndSleepUntilRunning();
}

void Network::postlude() {
	radio->hfCrystalClock->stop();
}

/*
 * If radio not already powered on, make it so.
 */
void Network::prepareToTransmitOrReceive() {
	if (!radio->isPowerOn()) {
			radio->powerOnAndConfigure();
			// TESTING: lower xmit power 8
			// radio->configureXmitPower(8);
		}
	assert(radio->isPowerOn());
	assert(radio->isDisabledState());	// not is receiving
}

void Network::startReceiving() {
	// Note radio might already be ready, but this ensure it.
	prepareToTransmitOrReceive();
	syncSleeper.clearReasonForWake();
	radio->receiveStatic();
	assert(!radio->isDisabledState());	// is receiving
}

void Network::stopReceiving() {
	if (radio->isPowerOn()) {
		radio->stopReceive();
	}
	assert(radio->isDisabledState());	// not is receiving
}

void Network::shutdown() {
	radio->powerOff();
	assert(!radio->isPowerOn());
}

