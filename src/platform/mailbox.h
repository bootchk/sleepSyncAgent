#include <inttypes.h>

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



// Work queue from app

/*
 * is queued a work message SyncAgent should broadcast?
 * SyncAgent will free unqueued msg.
 */
bool isQueuedWorkMsgFromApp();
uint8_t* unqueueWorkMsgFromApp();
void freeWorkMsg(void *);


// Received msg queue

/*
 * Get queued msg from Received queue.
 * SyncAgent will free the msg.
 *
 * If the platform does not implement a queue of received messages
 * (if the wireless stack disables receive after one message is received,
 * and the message is kept in one buffer in memory  )
 * then unqueueReceivedMsg() should return a pointer to the message,
 * and freeReceivedMsg() should do nothing.
 * e.g. nRF52
 */

/*
 * SyncAgent requires at least one byte of payload (MsgType)
 * and then knows length of message.
 * I.E. platform must ensure one byte, for security against buffer read-past.
 *
 * SyncAgent will call freeReceivedMsg() on msg pointer.
 */
bool isQueuedReceivedMsg();
uint8_t* unqueueReceivedMsg();
void freeReceivedMsg(void* msg);


// Work queue to app

/*
 * Platform must copy msg and enqueue it.
 * I.E. msg is not a queue element but is memory managed by SyncAgent.
 * (And app must free the copy after unqueuing it.)
 */
void queueWorkMsgToApp(void * msg, int length);


