#pragma once

#include <nRF5x.h>	// SystemID
#include "schedule.h"


/*
 *
 * Clique in smaller sense, knows:
 * - master
 * - schedule
 * - FUTURE: local, incomplete history of masters
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
	// This unit (with ID given by myID() ) is master of clique
	static void setSelfMastership();
	static void setOtherMastership(SystemID otherMasterID);
	static bool isSelfMaster();
	static bool shouldXmitSync();

	static bool isOtherCliqueBetter(SystemID otherMasterID);
	static bool isMsgFromMyClique(SystemID otherMasterID);



	/*
	 * Methods for determining whether we lost sync (master dropped out, or other reasons.)
	 */
	static void heardSync();
	static void checkMasterDroppedOut();
	// Clique is losing member that was Master
	static void onMasterDropout();

	/*
	 * Update clique from heard SyncMessage:
	 * - master <= SyncMessage
	 * - schedule <= offset of SyncMessage
	 */
	static void updateBySyncMessage(SyncMessage* msg);

	// static void initFromSyncMsg(SyncMessage* msg);
};
