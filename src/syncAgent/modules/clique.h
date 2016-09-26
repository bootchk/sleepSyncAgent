#pragma once

#include "../../platform/types.h"
#include "schedule.h"
#include "masterXmitSyncPolicy.h"

/*
 * Only my limited knowledge of the 'clique'.
 * I don't know all members.
 * I don't know accurate history of masters of clique.
 */
class Clique {

public:
	static MasterXmitSyncPolicy masterXmitSyncPolicy;
	static Schedule schedule;
	static SystemID masterID;	// self or other unit

	static void reset();
	static bool isSelfMaster();
	static void onMasterDropout();
	static void initFromSyncMsg(SyncMessage* msg);
	static bool isOtherCliqueBetter(SystemID otherMasterID);
};
