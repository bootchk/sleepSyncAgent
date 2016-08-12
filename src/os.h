
#include "message.h"

// OS abstraction layer
// Temp standin for OS
static void scheduleTask(void callback() ) {}
static void sleep() {}
static void turnReceiverOnWithCallback(void callback(Message) ) {}
static void turnReceiverOff() {}
static void xmit(MessageType msgType ) {}
static int myID() { return 1;}	// TI-RTOS see platform_
static bool isQueuedWorkMsg(){ return true;}
