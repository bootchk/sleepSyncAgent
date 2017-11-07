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

public:
	static Schedule schedule;

	static void init();

	/*
	 * Methods related to mastership and ID of clique.
	 */
	static SystemID getMasterID();
	// This unit (with ID given by System::ID() ) is master of clique
	static void setSelfMastership();
	static void setOtherMastership(SystemID otherMasterID);
	static bool isSelfMaster();
	static bool shouldTransmitSync();

	static bool isOtherCliqueBetter(SystemID otherMasterID);
	static bool isMsgFromMyClique(SystemID otherMasterID);



	/*
	 * Methods for determining whether we lost sync (master dropped out, or other reasons.)
	 */
	static void heardSync();
	static void checkMasterDroppedOut();
	// Self as slave failed to hear sync from  Master
	static void onMasterDropout();

	/*
	 * Update clique from heard SyncMessage:
	 * - master <= SyncMessage
	 * - schedule <= offset of SyncMessage
	 */
	static void updateBySyncMessage(SyncMessage* msg);


private:
	/*
	 * Behaviors on failing to hear sync
	 */
	// Brute force: self assume mastership
	static void grabMastership();

	static void revertToFormerMaster();

	// More nuanced
	static void revertByCliqueHistory();



	// static void initFromSyncMsg(SyncMessage* msg);
};
