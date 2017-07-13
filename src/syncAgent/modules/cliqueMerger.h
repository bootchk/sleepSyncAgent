#pragma once


#include "clique.h"
#include "mergeOffset.h"
#include "../message/message.h"

/*
 * Record of a Clique merging into a better Clique.
 *
 * Invariant: Role.isMerger => CliqueMerger.isActive()
 *
 * Singleton.
 *
 * Responsibilities
 * 1. know time of merge slot
 * 2. know the 'design' (state) of merger (attributes of the mergee clique.)
 *
 *
 */
class CliqueMerger {

public:
	/*
	 * Received a SyncMessage while fishing that means we will assumer role Merger.
	 */
	static void initFromMsg(SyncMessage* msg);
	static void deactivate();

	/*
	 * Adjusted schedule from a SyncMessage, which requires also adjust MergeOffset in this.
	 */
	static void adjustMergerBySyncMsg(SyncMessage* msg);

	/*
	 * Create a MergeSync message in the common message.
	 * !!! This must be called just before sending, since the SyncOffset is calculated when called.
	 */
	static SyncMessage* makeMergeSync();

	static const MergeOffset* getOffsetToMergeeSyncSlotMiddle();
};
