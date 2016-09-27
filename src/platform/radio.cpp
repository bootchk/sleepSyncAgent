

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


#include "../../config.h"
#include "radio.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

bool isReceiverOn() {return true; }
bool isTransmitterOn() {return true; }
void turnReceiverOn() {};
void turnReceiverOff() {}
void xmit(uint8_t* msg) {}

SystemID myID() { return 1;}

#endif

