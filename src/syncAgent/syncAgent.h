
#pragma once

// radioSoC
#include <services/mailbox.h>

// TODO eliminate this include
// SyncAgent callbacks for provisioning
#include "../syncAgentImp/provisioningPublisher.h"



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


/*
 * Bridge pattern: this is the external API.
 */





class SyncAgent {
public:

	/*
	 * Called by:
	 *  - external app (work provisioning)
	 *  - internal (network topology provisioning)
	 */
	static void subscribeProvisioning(PropertyIndex, ProvisionCallback);

	static void initSleepers();

	static void initSyncObjects(
			Mailbox* mailbox,
			void (*onWorkMsg)(unsigned char),
			void (*onSyncPoint)()
			);

	// Init Ensemble to hardcoded one of possibly many protocols
	static void initEnsembleProtocol();

	// Sleep until enough power to start syncing
	static void sleepUntilSyncPower();

	// execute protocol
	// with or without provisioning, according to build config
	static void loop() __attribute__ ((noreturn));

	// Is self Master of some clique (for now, only one clique.  Future: hops)
	static bool isSelfMaster();
	static bool isSelfSlave();

};

/*
 * FUTURE some cruft exists to support this.
 *
 * Now: SyncAgent puts itself in low-power mode but doesn't tell app.
 *
 * Future: SyncAgent tells app
 *
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


