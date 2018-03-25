
#pragma once

#include "../cliqueMerger/cliqueMerger.h"


// TODO ?? fishPolicy should be owned by SyncAgent.  fishPolicy used by clique and fishSchedule

/*
 * Bridge pattern: implementation of SyncAgent
 * Internal API defined here.
 */

class SyncAgentImp {

/*
 * Expose external API to SyncAgent class, which delegates here.
 */
public:

	static void initSleepers();

	static void initSyncObjects(
			Mailbox* mailbox,
			void (*onWorkMsg)(MailContents),
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


	/*
	 * Internal API
	 */


	// Some of data members: see also anon namespaces for other owned objects
private:
	// DYNAMIC static uint8_t receiveBuffer[Radio::MaxMsgLength];
	// FIXED: Radio owns fixed length buffer

	// FUTURE: global instead of public?
public:	// to SyncSlot mainly
	static CliqueMerger cliqueMerger;

	static MailContents countMergeSyncHeard ;

private:
	static void preludeToLoop();
	static void preludeToSyncSlot();

	// Interface towards app
	static void (*onWorkMsgCallback)(MailContents);
	static void (*onSyncPointCallback)();
	// FUTURE static void (*onSyncingPausedCallback)();	// callback to app when syncing is paused


	// methods
public:

	// Upstream communication to app
	static void relayHeardWorkToApp(Payload work);

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

	static uint32_t getPhase();
	static uint32_t getReasonForWake();

	/*
	 * More or less: reset to starting conditions.
	 */
	static void scatter();
};
