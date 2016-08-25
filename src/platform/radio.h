

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */

#include "../types.h"

bool isReceiverOn();
bool isTransmitterOn();
void turnReceiverOn();
void turnReceiverOff();
// TODO length xmit
void xmit(uint8_t* msg);	// octets
SystemID myID();


