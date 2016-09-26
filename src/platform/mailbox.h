

/*
 * Platform must provide three queues of messages:
 * - work in (SyncAgent will send during work slot
 * - work out (SyncAgent received during work slot
 * - received messages in
 *
 * Queue protocol: queue a pointer, returns a pointer, caller must free() what is pointed to.
 *
 * !!! Platform may implement trivial queue:
 * On some platforms, the receiver is off after a message is received.
 * The one buffer the receiver wrote is the 'queue' and freeReceivedMsg() does nothing.
 *
 * First phase of implementation:
 * -platform may stub out work queue and do no useful work.
 * -platform implements trivial received queue (the single buffer the radio peripheral writes to)
 */

// TODO queue only the payload, SyncAgent wraps in protocol header



// Work queues

// is work message SyncAgent should broadcast?
bool isQueuedWorkMsgFromApp();

/*
 * Get queued msg from Work queue.
 * SyncAgent will free the msg.
 */
void* unqueueWorkOutMsg();
void freeWorkMsg();
// TODO never called?

/*
 * Platform must copy msg.
 * App must free the copy.
 */
bool queueWorkMsgToApp(void * msg, int length);


// Received msg queue

/*
 * Get queued msg from Received queue.
 * SyncAgent will free the msg.
 */
void* unqueueReceivedMsg();


void freeReceivedMsg(void* msg);


