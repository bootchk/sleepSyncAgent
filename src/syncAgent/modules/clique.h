#pragma once

#include "../../platform/types.h"
#include "schedule.h"
#include "policy/masterXmitSyncPolicy.h"

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
 */
class Clique {
private:
	static SystemID masterID;	// self or other unit

public:
	static Schedule schedule;
	static MasterXmitSyncPolicy masterXmitSyncPolicy;

	static SystemID getMasterID() { return masterID; }
	static void setSelfMastership();
	static void setOtherMastership(SystemID otherMasterID);
	static bool isSelfMaster();
	static bool isOtherCliqueBetter(SystemID otherMasterID);
	static bool isMyMaster(SystemID otherMasterID);

	// New clique
	static void reset();

	// Clique is losing member that was Master
	static void onMasterDropout();

	// Clique is changing from heard SyncMessage
	static void changeBySyncMessage(SyncMessage* msg);

	// static void initFromSyncMsg(SyncMessage* msg);
};
