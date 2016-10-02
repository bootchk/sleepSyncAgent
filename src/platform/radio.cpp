

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


#include "../../config.h"
#include "radio.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

void Radio::init(void (*onRcvMsgCallback)()) {}
bool Radio::isDisabled() {return false; }

void Radio::powerOnAndConfigure() {}
void Radio::powerOff() {}
bool Radio::isPowerOn() {return true; }

void Radio::transmitStaticSynchronously() {}
void Radio::receiveStatic() {}
void Radio::stopReceive() {}

uint8_t* Radio::getBufferAddress() {return nullptr; }

#endif

