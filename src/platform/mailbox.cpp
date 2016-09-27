
#include "mailbox.h"


#ifndef SYNC_AGENT_IS_LIBRARY

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
