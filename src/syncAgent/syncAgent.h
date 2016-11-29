
#pragma once

#include "modules/message.h"
#include "modules/cliqueMerger.h"
#include "modules/role.h"
#include "syncPeriod/syncPeriod.h"


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

// data members
private:
	static bool isSyncingState;
	// DYNAMIC static uint8_t receiveBuffer[Radio::MaxMsgLength];
	// FIXED: Radio owns fixed length buffer

public:	// to SyncSlot mainly
	static CliqueMerger cliqueMerger;
private:
	// syncPeriod and powerManager local to syncAgentLoop.c
	static LEDLogger ledLogger;

	// Interface towards app
	static void (*onWorkMsgCallback)(WorkPayload);
	static void (*onSyncPointCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused




// methods
public:
	static void init( Radio* radio,
			Mailbox* mailbox,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);
	static void loop();

	static void startSyncing();
	static void doSyncPeriod();

public:
	static void relayWorkToApp(WorkPayload work);

	static void toMergerRole(SyncMessage* msg);
	static void mangleWorkMsg(SyncMessage* msg);
	static void toFisherRole();

private:
	static void pauseSyncing();
	static void doDyingBreath();
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


