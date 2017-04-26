
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
 * If radio not already configured, make it so.
 * And insure radio is ready (state==disabled)
 */
void Network::prepareToTransmitOrReceive() {
	if (!radio->isConfigured()) {
			radio->resetAndConfigure();
			// TESTING: lower xmit power 8
			// radio->configureXmitPower(8);
		}
	assert(!radio->isInUse());
}

void Network::startReceiving() {
	// Note radio might already be ready, but this ensure it.
	prepareToTransmitOrReceive();
	syncSleeper.clearReasonForWake();
	radio->receiveStatic();
	assert(radio->isInUse());
}

void Network::stopReceiving() {
	if (radio->isInUse()) {
		radio->stopReceive();
	}
	assert(!radio->isInUse());
}

/* OLD
void Network::shutdown() {
	radio->powerOff();
	assert(!radio->isPowerOn());
}
*/
