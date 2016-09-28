

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


#include "../../config.h"
#include "radio.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

void Radio::init(void (*onRcvMsgCallback)()) {}
bool Radio::isDisabled() {return false; }

void Radio::powerOn() {}
void Radio::powerOff() {}
bool Radio::isPowerOn() {return true; }

void Radio::transmit(uint8_t* data, uint8_t length) {}
void Radio::receive(uint8_t* data, uint8_t length) {}

#endif

