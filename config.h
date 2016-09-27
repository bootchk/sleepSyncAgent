/*
 * Configure:
 * - optional behaviour of SyncAgent
 * - library build
 */

/*
 * Define this if SyncAgent should conserve power.
 *
 * Yes:
 * SyncAgent checks Vcc and will not maintain sync or convey work if Vcc is low.
 * SyncAgent will attempt to recover last known sync when power is sufficient.
 *
 * Platform must implement getVcc()
 *
 * No:
 * SyncAgent maintains sync even to the point of brownout.
 * Upon brownout exception (which app handles) typically mcu resets,
 * all volatile memory is lost, including sync.
 *
 * isPowerForRadio() is stubbed to always return true.
 *
 * Platform layer need not implement getVcc()
 */
//#define SYNC_AGENT_CONSERVE_POWER 1


/*
 * Define this if SyncAgent should convey work messages to app.
 *
 * Yes:
 * SyncAgent unqueues and enqueues to work queues from platform.
 *
 * No:
 * SyncAgent implements a work slot,
 * but since no units are sending work,
 * never hears a work message.
 * SyncAgent enqueues and dequeues are stubbed to no ops.
 * Platform layer need not implement work queue operations.
 */
#define SYNC_AGENT_CONVEY_WORK 1


/*
 * Define this if SyncAgent is built as a library.
 *
 * Yes:
 * SyncAgent expects platform to implement certain functions (undefined in the library.)
 *
 * No:
 * Stubs are compiled in so project builds and links cleanly
 * (but it is non-functional.)
 */
//#define SYNC_AGENT_IS_LIBRARY 1
