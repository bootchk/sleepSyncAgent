
#pragma once

#include "cliqueMerger/cliqueMerger.h"
#include "message/message.h"
#include "provisioningPublisher.h"



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


// TODO fishPolicy should be owned by SyncAgent
/*
 * fishPolicy used by clique and fishSchedule

#include "policy/fishPolicy.h"
//extern SimpleFishPolicy fishPolicy;
extern SyncRecoveryFishPolicy fishPolicy;
 */





class SyncAgent {

// Some of data members: see also anon namespaces for other owned objects
private:
	// DYNAMIC static uint8_t receiveBuffer[Radio::MaxMsgLength];
	// FIXED: Radio owns fixed length buffer

// FUTURE: global instead of public?
public:	// to SyncSlot mainly
	static CliqueMerger cliqueMerger;

	static WorkPayload countMergeSyncHeard ;

private:
	// syncPeriod local to syncAgentLoop.c

	// Interface towards app
	static void (*onWorkMsgCallback)(WorkPayload);
	static void (*onSyncPointCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused

	static ProvisionCallback onProvisionedCallback;

	// methods
public:

	// Upstream communication to app
	static void relayHeardWorkToApp(WorkPayload work);

	static void subscribeProvisioning(PropertyIndex, ProvisionCallback);

	static void initSleepers();

	static void initSyncObjects(
			Mailbox* mailbox,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			);

	// Init Ensemble to hardcoded one of possibly many protocols
	static void initEnsembleProtocol();

	// Sleep until enough power to start syncing
	static void sleepUntilSyncPower();

	// execute protocol
	// with or without provisioning, according to build config
	static void loop() __attribute__ ((noreturn));


	/*
	 * Actions for state/mode/role transitions
	 */
	static void toMergerFromFisher(SyncMessage* msg);
	static void toFisherFromMerger();
	static void toFisherFromNoFishing();
	static void ToNoFishingFromOther();
	/*
	 * Normal completion, or abandoning.
	 */
	static void stopMerger();

	// Is self Master of some clique (for now, only one clique.  Future: hops)
	static bool isSelfMaster();

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


