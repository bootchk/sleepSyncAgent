

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */

#include "../syncAgent/message.h"

bool isReceiverOn();
bool isTransmitterOn();
void turnReceiverOn();
void turnReceiverOff();
void xmit(SyncMessage* msg);
void xmit(WorkMessage* msg);
SystemID myID();

