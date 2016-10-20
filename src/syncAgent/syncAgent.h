
#pragma once

#include "../augment/powerManager.h"
#include "../platform/platform.h"	// Radio, Sleeper, LEDLogger

#include "modules/message.h"
#include "modules/clique.h"
#include "modules/policy/dropoutMonitor.h"
#include "modules/policy/mergePolicy.h"
#include "modules/cliqueMerger.h"
#include "modules/role.h"
#include "modules/serializer.h"

#include "slots/syncSlot.h"
#include "slots/workSlot.h"

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
	static Sleeper sleeper;

public:	// to SyncSlot mainly
	static CliqueMerger cliqueMerger;
	static Role role;
private:
	static MergePolicy mergePolicy;

	static Serializer serializer;
	static PowerManager powerMgr;

	static SyncSlot syncSlot;
	static WorkSlot workSlot;


	static LEDLogger ledLogger;

	// Owned by app
	static Radio* radio;
	static void (*onWorkMsgQueuedCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused

	static uint32_t countValidReceives;
	static uint32_t countInvalidTypeReceives;
	static uint32_t countInvalidCRCReceives;

// methods

	static void startSyncing();
	static void doSyncPeriod();
public:
	// TODO belong here?
	static bool dispatchMsgUntil(
			DispatchFuncPtr,
			OSTime (*func)());
	static void relayWorkToApp(WorkMessage* msg);
private:
	static bool dispatchMsg(DispatchFuncPtr);



	// work slot starts without an event
	static void startFishSlot();

	// end of slots

	static void endFishSlot();
	// Merge over without event, as soon as xmit


	// dispatch

	static bool dispatchMsgReceivedInFishSlot(SyncMessage* msg);
	// Merge slot only xmits, not receive messages

	// sync


	// fish
	static void doFishSlot();


	static void pauseSyncing();
	static void doDyingBreath();

	// transmissions
	// Depending on OS, might be asynchronous (no waiting)
	static void xmitRoleAproposSync();


	// msg handlers: messageType x slotType, with omissions


	static void doMasterSyncMsgInFishSlot(SyncMessage* msg);





	// merge
	static void toMergerRole(SyncMessage* msg);
	static void endMergerRole();
	static void doMergeSlot();
	static void sendMergeSync();
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


