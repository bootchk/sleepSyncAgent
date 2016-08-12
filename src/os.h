
#include "message.h"

// Temp standin for OS
static void scheduleTask(void callback() ) {}
static void sleep() {}
static void turnRadioOnWithCallback(void callback(Message) ) {}
static void xmit(MessageType msgType ) {}
static int myID() { return 1;}	// TI-RTOS see platform_
