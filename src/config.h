/*
 * Configure:
 * - optional behaviour of SyncAgent
 * - library build
 */

//TODO rename idle near brownout
/*
 * Define this if SyncAgent should conserve power
 *
 * Yes:
 * SyncAgent checks Vcc and will not transmit sync or convey work if Vcc is low.
 * SyncAgent still keeps the sync schedule, but it drifts (without sync messages.
 * When power recovers to sufficient, SyncAgent will resume xmitting sync on its drifted schedule,
 * which might lead to more rapid sync with other units.
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
/*
 * FUTURE related to SIMPLE_SYNC_PERIOD.
 * If yes, at some power level SyncAgent stops xmitting work in a separate work slot
 * (saving 50%, using only 2 active slots instead of 3.)
 *
 * FUTURE if power is low, SyncAgent could xmit and listen for sync, but not fish.
 * In that case, it would stay in sync, but not find new mobile units.
 * Again saving roughly speaking another 50% power.
 *
 * The design now is that the app takes responsibility for not xmitting work
 * if there is not enough power to do work.
 *
 * Some units may have enough power to xmit work,
 * and an insufficient powered unit may hear the work
 * (Depending on SIMPLE_SYNC_PERIOD, whether separate work slot is executed to listen for work)
 * and convey the work to the insufficiently powered app.
 */
#define SYNC_AGENT_CONSERVE_POWER 1




/*
 * Define equal to 1 if this if SyncAgent should convey work messages to app.
 *
 * Yes:
 * SyncAgent unqueues and enqueues to work queues to/from app.
 *
 * No:
 * SyncAgent does not implement a work slot.
 * SyncAgent work enqueues and dequeues are stubbed to no ops.
 * Platform layer need not implement work queue operations.
 * The app doesn't communicate work with other units.
 *
 * All the app does is receive a callback at each SyncPoint.
 * The callback should be a lower priority process than the SyncAgent (or kept short.)
 */
#define SYNC_AGENT_CONVEYS_WORK 1


/*
 * Does SyncAgent convey work in separate work slot?
 * See ramifications in the code.
 *
 * Yes: separate work slot
 * No: work conveyed in combined work/sync slot
 */
//#define SIMPLE_SYNC_PERIOD 1


/*
 * Define if SyncAgent is built as a library.
 * Building as library is 'for release' but uses cross-compiling Eclipse build configuration e.g. ArchiveM0
 * SyncAgent expects platform to implement certain functions (undefined in the library.)
 *
 * If not defined, change Eclipse build configuration to "Debug"
 * Yields a clean compile, non-functional executable for the host.
 * Stubs are compiled in so project builds and links cleanly
 * (but it is non-functional.)
 */
#define SYNC_AGENT_IS_LIBRARY 1



/*
 * Define this if, on the platform, the radio peripheral is synchronous to the SyncAgent.
 *
 * Note the radio peripheral is usually an independent peripheral/device,
 * but the platform may layer it so that messages are synchronous.
 * A sleeping SyncAgent only means the mcu is idle, the radio peripheral may still be active.
 *
 * Yes: (synchronous)
 * After platform receives msg, radio is disabled until SyncAgent starts it again.
 * When platform receives msg:
 * - sleeping sync agent wakes because it was sleeping on ANY event
 * - SyncAgent can know the reason for wakening by calling isWakeReasonMsgReceived()
 * - SyncAgent must read the received message before restarting the receiver.
 *
 *
 * No:
 * Radio peripheral is run in a separate thread of an RTOS
 * After platform receives msg, radio continues to receive without intervention by SyncAgent,
 * whether or not SyncAgent keeps up.
 * When platform receives msg:
 *  - it is queued
 *  - sleeping sync agent wakes because it was blocked on the queue
 */
#define RECEIVE_IS_SYNCHRONOUS 1


/*
 * Comment this out if you want a unit with a large ID to be master of units with smaller IDs.
 *
 * Thus if only one of your units is a DK (with debugging capability)
 * configure this back and forth
 * to test both master and slave side of a clique.
 */
#define LEAST_ID_IS_BETTER_CLIQUE 1

