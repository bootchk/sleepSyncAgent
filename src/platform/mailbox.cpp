
#include "mailbox.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

void Mailbox::put(uint32_t aItem){ }
uint32_t Mailbox::fetch(){ return 1; }
bool Mailbox::isMail(){ return false; }

#endif
