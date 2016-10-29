
#include "message.h"
#include "clique.h"

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
	static DeltaTime offsetToMergee;

public:
	static void initFromMsg(SyncMessage* msg);
	static void deactivate();

	static void adjustMergerBySyncMsg(SyncMessage* msg);

	static void makeMergeSync(SyncMessage& msg);

	static void setOffsetToMergee(DeltaTime offset);
	static DeltaTime getOffsetToMergee();

private:
	static void initMergeMyClique(SyncMessage* msg);
	static void initMergeOtherClique();
};
