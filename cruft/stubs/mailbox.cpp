
#include "mailbox.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

void Mailbox::put(WorkPayload aItem){ (void) aItem; }
WorkPayload Mailbox::fetch(){ return 1; }
bool Mailbox::isMail(){ return false; }

#endif
