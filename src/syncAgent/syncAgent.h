

#include "../augment/powerManager.h"
#include "../platform/platform.h"	// Radio, Sleeper, LEDLogger

#include "modules/message.h"
#include "modules/clique.h"
#include "modules/policy/dropoutMonitor.h"
#include "modules/policy/mergePolicy.h"
#include "modules/cliqueMerger.h"
#include "modules/role.h"
#include "modules/serializer.h"

/*
 * SyncAgent manages sleep synchronization for wireless network.
 * System is low-power, sleeping much of the time.
 *
 * A task/thread that never returns.
 * Collaborates with WirelessStack task (higher priority) and Work task (lower priority.)
 *
 * SyncAgent powers radio off and on.
 * With radio receiver on, SyncAgent sleeps, waking on events from WirelessStack or Timer.
 *
 * A work thread sends work messages through SyncAgent via a queue.
 * SyncAgent delivers received work messages to work thread via a queue.
 *
 * Uses PowerManager.
 * SyncAgent is always cycling through sync periods,
 * but when not enough power, does not use radio to keep in sync.
 * When power is restored, sync periods MIGHT still be in sync.
 *
 * Singleton: all members static, no this.
 *
 * Does not use heap.
 */

// typedef (bool (*dispatchQueuedMsg)(Message *)) DispatchFuncPtr;
typedef bool (*DispatchFuncPtr)(SyncMessage *) ;

class SyncAgent {

public:
	static void init( Radio* radio, void (*onWorkMsgQueued)() );
	static void loop();

private:

// data members
	static bool isSyncingState;
	// DYNAMIC static uint8_t receiveBuffer[Radio::MaxMsgLength];
	// FIXED: Radio owns fixed length buffer

	// has-a, all singletons
	static Clique clique;
	static DropoutMonitor dropoutMonitor;
	static CliqueMerger cliqueMerger;
	static MergePolicy mergePolicy;
	static Role role;
	static Serializer serializer;
	static PowerManager powerMgr;
	static Sleeper sleeper;

	static LEDLogger ledLogger;

	// Owned by app
	static Radio* radio;
	static void (*onWorkMsgQueuedCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused

// methods

	static void startSyncing();
	static void doSyncPeriod();
	static bool dispatchMsgUntil(
			DispatchFuncPtr,
			OSTime (*func)());
	static bool dispatchMsg(DispatchFuncPtr);

	// start of slots
	static void startSyncSlot();
	// work slot starts without an event
	static void startFishSlot();

	// end of slots
	static void endSyncSlot();
	static void endWorkSlot();
	static void endFishSlot();
	// Merge over without event, as soon as xmit


	// dispatch
	static bool dispatchMsgReceivedInSyncSlot(SyncMessage* msg);
	static bool dispatchMsgReceivedInWorkSlot(SyncMessage* msg);
	static bool dispatchMsgReceivedInFishSlot(SyncMessage* msg);
	// Merge slot only xmits, not receive messages

	// sync
	static void doSyncSlot();
	static void doMasterSyncSlot();
	static void doSlaveSyncSlot();
	static bool doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)());
	static void doIdleSlotRemainder();
	static bool shouldTransmitSync();
	static void transmitMasterSync();
	static void makeCommonMasterSyncMessage();


	// work
	static void doWorkSlot();
	static void startWorkSlot();
	static void relayWorkToApp(WorkMessage* msg);

	// fish
	static void doFishSlot();

	static bool isSyncFromBetterMaster(SyncMessage* msg);
	static void changeMaster(SyncMessage* msg);
	static void pauseSyncing();
	static void doDyingBreath();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();
	static void xmitAproposWork();
	static void xmitWork();

	// msg handlers: messageType x slotType, with omissions
	static bool doSyncMsgInSyncSlot(SyncMessage* msg);	//MasterSync OR MergeSync

	static void doMasterSyncMsgInFishSlot(SyncMessage* msg);
	static void doAbandonMastershipMsgInSyncSlot(SyncMessage* msg);
	static void doWorkMsgInSyncSlot(WorkMessage* msg);
	static void doWorkMsgInWorkSlot(WorkMessage* msg);

	// merge
	static void toMergerRole(SyncMessage* msg);
	static void endMergerRole();
	static void doMergeSlot();
	static void sendMerge();


	// abandon
	static void tryAssumeMastership(SyncMessage* msg);
};

/*
 * OBSOLETE
 * App calls startSyncing on mcu power on reset POR.
 * App can not stop synching.
 * SyncAgent monitors power and tells app onSyncLostCallback.
 * App continues with mcu in low power, radio not on.
 * When power is restored, app calls SyncAgent.resume()
 *
 * Proper sequence:  POR, startSynching, onSyncingPausedCallback, resumeSyncing, onSyncingPausedCallback, resumeSyncing,...
 */

//static void resumeAfterPowerRestored();


