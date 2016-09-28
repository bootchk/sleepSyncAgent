
#include "../../config.h"
#include "mailbox.h"


// if library and
#if SYNC_AGENT_IS_LIBRARY==1  && SYNC_AGENT_CONVEY_WORK==1

// platform implements queues

#else

// Is library but no conveyance of work
// Stub out calls to work queues

// stubs
// is...Msg returning false means SyncAgent will not call unqueuing and freeing functions

bool isQueuedWorkMsgFromApp() { return false; }
void* unqueueWorkMsgFromApp() { return nullptr; }
void freeWorkMsg(void*) {}

//bool isQueuedWorkOutMsg(){ return false; }
//void* unqueueMsg(){ return nullptr; }

bool isQueuedReceivedMsg() { return false; }
void* unqueueReceivedMsg() { return nullptr; }
void freeReceivedMsg(void* msg) {};


void queueWorkMsgToApp(void * msg, int length) {}


#endif
// else platform provides
