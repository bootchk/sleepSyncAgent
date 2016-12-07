
#include "message.h"
#include "clique.h"
#include "../mergeOffset.h"

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

private:
	/*
	 * Private state, redundant to outside logic about role.
	 * Invariant: true => role is Merger
	 * outside logic calls deactivate
	 */
	static bool isActive;

	static SystemID masterID;
	static Clique* owningClique;	// 2-way relation: Clique owns CliqueMerger, CliqueMerger uses owning Clique

	// attributes of mergee clique, offset relative to owning clique's current schedule
	static MergeOffset offsetToMergee;

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
	static void makeMergeSync(SyncMessage& msg);

	// Constraints enforced on setting
	static void setOffsetToMergee(DeltaTime offset);
	static MergeOffset getOffsetToMergee();

private:
	static void initMergeMyClique(SyncMessage* msg);
	static void initMergeOtherClique();
};
