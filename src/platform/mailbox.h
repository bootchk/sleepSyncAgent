

/*
 * Queue protocol: queue a pointer, returns a pointer, caller must free what is pointed to.
 */
bool isQueuedInMsg();
bool isQueuedWorkOutMsg();
void freeMsg(void* msg);
void* unqueueMsg();
