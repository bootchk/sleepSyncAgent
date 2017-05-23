
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
	assert(radio->isConfigured());
}

void Network::shutdown() {
	radio->hfCrystalClock->stop();
}



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

