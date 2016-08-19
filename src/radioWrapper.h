

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */

#include "syncAgent/message.h"

//static void sleep() {}
static void turnReceiverOnWithCallback(void callback(SyncMessage) ) {}
static void turnReceiverOff() {}
static void xmit(SyncMessage msg) {}
static void xmit(WorkMessage msg) {}
static SystemID myID() { return 1;}	// TI-RTOS see platform_  MAC of my radio
