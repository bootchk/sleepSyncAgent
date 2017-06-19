
#pragma once

#include "modules/syncPowerManager.h"
#include "modules/message.h"
#include "modules/cliqueMerger.h"



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



class SyncAgent {

// Some of data members: see also anon namespaces for other owned objects
private:
	// DYNAMIC static uint8_t receiveBuffer[Radio::MaxMsgLength];
	// FIXED: Radio owns fixed length buffer

// FUTURE: global instead of public?
public:	// to SyncSlot mainly
	static CliqueMerger cliqueMerger;

private:
	// syncPeriod local to syncAgentLoop.c

	// Interface towards app
	static void (*onWorkMsgCallback)(WorkPayload);
	static void (*onSyncPointCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused




// methods
public:
	static void initSleepers(SyncPowerManager*, LongClockTimer*);

	static void initSyncObjects(
			Mailbox* mailbox,
			SyncPowerManager* aSyncPowerManager,
			LongClockTimer* aLongClockTimer,
			BrownoutManager* aBrownoutManager,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);

	// Sleep until enough power to start syncing
	static void sleepUntilSyncPower();

	static void loop() __attribute__ ((noreturn));


	// These  methods are called from below (friends?)
	static void relayHeardWorkToApp(WorkPayload work);

	static void toMergerRole(SyncMessage* msg);
	static void toFisherRole();

	static uint32_t getPhase();
	static uint32_t getReasonForWake();
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
// static void startSyncing();


