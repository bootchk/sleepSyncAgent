#pragma once


#include "../clique/clique.h"
#include "../clique/periodTime.h"
#include "../message/message.h"


/*
 * Record of a Clique merging into a better Clique.
 *
 * Invariant: Role.isMerger => CliqueMerger.isActive()
 *
 * Singleton.
 *
 * Responsibilities
 * 1. know PeriodTime of merge slot
 * 2. know the 'design' (state) of merger (attributes of the mergee clique.)
 * 3. know how to contstruct MergeSync msg
 *
 *
 */
class CliqueMerger {

public:
	/*
	 * Received a SyncMessage while fishing.
	 * Will assumer role Merger, but this only activates a record.
	 */
	static void initFromMsg(SyncMessage* msg);

	/*
	 * Does not stop Merger role, just marks record no longer used.
	 */
	static void deactivate();

	/*
	 * Adjusted schedule from a SyncMessage, which requires also adjust MergeOffset in this.
	 */
	static void adjustMergerBySyncMsg(SyncMessage* msg);

	/*
	 * Knows how construct.
	 * !!! This must be called just before sending, since the SyncOffset is calculated when called.
	 */
	static SyncMessage* makeMergeSyncMsg();

	// const pointer to const PeriodTime
	static const PeriodTime* getPeriodTimeToMergeSlotStart();
};
