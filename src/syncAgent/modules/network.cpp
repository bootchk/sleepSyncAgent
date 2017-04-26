
#include <cassert>

#include "../globals.h"
#include "network.h"



bool Network::isConfigured(){
	return radio->isConfigured();
}

bool Network::isLowPower() {
	return ((!radio->isInUse()) && !radio->hfCrystalClock->isRunning());
}

bool Network::isRadioInUse() {
	return radio->isInUse();
}

void Network::startup() {
	radio->hfCrystalClock->startAndSleepUntilRunning();
	// TODO superfluous
	// prepareToTransmitOrReceive();
	assert(radio->isConfigured());
}

void Network::shutdown() {
	radio->hfCrystalClock->stop();
}

/*
 * If radio not already configured, make it so.
 * And insure radio is ready (state==disabled)
 *
 * TODO This is superfluous, the radio stays configured
 */
/*
void Network::prepareToTransmitOrReceive() {
	if (!radio->isConfigured()) {
			radio->resetAndConfigure();
			// TESTING: lower xmit power 8
			// radio->configureXmitPower(8);
		}
	assert(!radio->isInUse());
}
*/

void Network::startReceiving() {
	// Note radio might already be ready, but this ensure it.
	// OLD prepareToTransmitOrReceive();
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

