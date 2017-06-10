
#include <cassert>

#include "../globals.h"
#include "network.h"



bool Network::isConfigured(){
	return radio->isConfigured();
}

bool Network::isLowPower() {
	return ((!radio->isPowerOn()) && !radio->hfCrystalClock->isRunning());
}

bool Network::isRadioInUse() {
	return radio->isInUse();
}

/*
 * Startup must be called before StartReceiving or Transmit (TODO no Transmit in NetworkAPI)
 */
void Network::startup() {

	// enable this first so it has time to ramp up
	// assert enough power for radio => >2.1V required by DCDCPowerSupply
	radio->dcdcPowerSupply->enable();

	radio->powerOn();
	// Some platforms require config after powerOn()
	radio->configurePhysicalProtocol();

	radio->hfCrystalClock->startAndSleepUntilRunning();
	assert(radio->isConfigured());
}

void Network::shutdown() {
	radio->hfCrystalClock->stop();

	radio->powerOff();

	// disable because Vcc may be below what DCDCPowerSupply requires
	radio->dcdcPowerSupply->disable();
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

