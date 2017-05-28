
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

/*
 * Startup must be called before StartReceiving or Transmit (TODO no Transmit in NetworkAPI)
 */
void Network::startup() {
	radio->hfCrystalClock->startAndSleepUntilRunning();
	assert(radio->isConfigured());
}

void Network::shutdown() {
	radio->hfCrystalClock->stop();
}



void Network::startReceiving() {
	/*
	 * Not log to flash here, since it takes too long.
	 */

	// TODO should this be in caller?
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

