

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


#include "radio.h"

#ifndef PLATFORM_TIRTOS
// stubs

bool isReceiverOn() {return true; }
bool isTransmitterOn() {return true; }
void turnReceiverOn() {};
void turnReceiverOff() {}
void xmit(uint8_t* msg) {}

SystemID myID() { return 1;}

#else
// TIRTOS

// Receiver is a BT GAP Observer Role.  Start discovery session (scan)
void turnReceiverOn() {
	GAPObserverRole_StartDiscovery(foo);	// TODO
}

void turnReceiverOff() {
	GAPObserverRole_CancelDiscovery();
}

void xmit(msg) {
	startAdvertising();
	delay short time
	stopAdvertising();
}

SystemID myID() {
	// 48-bit MAC of radio
	// This implementation's result may vary depending on other things.
	// e.g. whether you called a GAP function to set the ID.
	// Alternatively, you can directly read what is burned in ROM (FCFG)
	// See TIForum "registers to read BT address of cc26xx"
	unit64_t ownAddress;
	GAPRole_GetParameter(GAPROLE_BD_ADDR, &ownAddress);
	return ownAddress;
	// see platform_  ??? function
}

#endif
