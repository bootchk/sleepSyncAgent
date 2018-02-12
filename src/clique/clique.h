#pragma once

#include <radioSoC.h>	// SystemID
#include "schedule.h"


/*
 *
 * Clique in smaller sense, knows:
 * - master
 * - schedule
 * I.E. is only the local representation of the larger clique,
 * Different unit's representations of the same larger clique may differ.
 * The larger clique might not even exist anymore.
 *
 * Clique in larger sense: a set of units on the same schedule having same master, knows:
 * - all members.
 * - complete history tree of masters of clique (only the masters on self's branch)
 *
 * This only implements the smaller sense.
 */
class Clique {

private:
	// Self (with ID given by System::ID() ) is master of clique
	static void setSelfMastership();
	// Other is master.  Other might be self
	static void updateMastership(SystemID otherMasterID);

public:
	static Schedule schedule;

	static void init();

	/*
	 * Methods related to mastership and ID of clique.
	 */
	static SystemID getMasterID();
	static bool isSelfMaster();

	static bool isOtherCliqueBetter(SystemID otherMasterID);
	static bool isMsgFromMyClique(SystemID otherMasterID);

	/*
	 * Role transitions.
	 */
	static void assumeMastership();
	// Current design doesn't distinguish when this transition occurs
	// static void mergeMastership();


	/*
	 * sync transmit
	 */
	static bool shouldTransmitSync();
private:
	static void resetTransmitSyncPolicy();
public:

	/*
	 * Determining whether we lost sync (master dropped out, or other reasons.)
	 */
	static void heardSync();
	static void checkMasterDroppedOut();

	/*
	 * On provisioned to scatter,
	 * This is strong alternative of scatter: become master of my own clique
	 * and randomize my syncPoint (slightly or a lot)
	 */
	static void scatterSync();

// TODO, update workTime so that we continue to flash
// at same wall time of clique

	/*
	 * Update clique from heard SyncMessage:
	 * - master <= SyncMessage
	 * - schedule <= offset of SyncMessage
	 */
	static void updateBySyncMessage(SyncMessage* msg);


private:
	/*
	 * Behaviors on lost sync (failing to hear)
	 */
	// Self as slave lost sync from  Master
	static void onMasterDropout();

	// Brute force: self assume mastership
	static void grabMastership();

	static void revertToFormerMaster();

	// More nuanced
	static void revertByCliqueHistory();



	// static void initFromSyncMsg(SyncMessage* msg);
};
