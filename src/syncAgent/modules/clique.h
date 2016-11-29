#pragma once

#include "../../platform/types.h"
#include "schedule.h"

#include "../policy/dropoutMonitor.h"
//#include "../policy/masterXmitSyncPolicy.h"
#include "../policy/adaptiveXmitSyncPolicy.h"

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
private:
	static SystemID masterID;	// self or other unit

public:
	static Schedule schedule;

	static DropoutMonitor dropoutMonitor;

	// Choices:
	//static MasterXmitSyncPolicy masterXmitSyncPolicy;
	static AdaptiveXmitSyncPolicy masterXmitSyncPolicy;

	static SystemID getMasterID() { return masterID; }
	static void setSelfMastership();
	static void setOtherMastership(SystemID otherMasterID);
	static bool isSelfMaster();
	static bool isOtherCliqueBetter(SystemID otherMasterID);
	static bool isMsgFromMyClique(SystemID otherMasterID);

	// New clique
	static void reset();

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
