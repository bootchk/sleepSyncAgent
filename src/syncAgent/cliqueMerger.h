
#include "../message.h"
#include "clique.h"

/*
 * Record of a Clique merging into a better Clique.
 *
 * Invariant: Role.isMerger => CliqueMerger.isActive()
 *
 * Singleton.
 *
 * Responsibilities
 * 1. schedule time for merge slot
 * 2. know the 'design' (state) of merger (attributes of the mergee clique.)
 *
 * CliqueMerger knows when to schedule a mergerSlot (with collision avoidance).
 * Note that many fishers may discover the same otherClique, try to merge it, and thus contend.
 * A unit sends MergeSync probablistically, to avoid contention.
 * The contention is with:
 * - master of otherClique (still sending Sync in its syncSlot)
 * - other mergers of otherClique
 *
 * Current design/implementation:
 *
 * A unit only sends one MergeSync.
 * If master of otherClique fails to hear, they will keep mastering the other clique,
 * and slaves of otherClique that failt to hear will remain in the other clique.
 * If master of otherClique hears but slaves of otherClique fail to hear,
 * slaves will fallout of sync, and resume mastership of otherClique
 * ( possibly remain in their own single member clique.)
 * In both cases the clique(s) must be fished again.
 *
 * Note however, that multiple units may have fished the same otherClique
 * and may be contending to merge it.
 *
 * Alternative implementation:
 *
 * Each unit repeatedly send MergeSync to increase chance other clique is merged quickly.
 */
class CliqueMerger {

public:
	static bool isActive;	// flag, no getter/setter

private:
	// attributes of otherClique
	static DeltaTime offsetToMergee;	// in current schedule
	static int masterID;

	static Clique* owningClique;	// 2-way relation: Clique owns CliqueMerger, CliqueMerger uses owning Clique
	//static int notifyCountdown;

public:
	static void initFromMsg(Message msg);


	static void adjustBySyncMsg(Message msg);
	static bool shouldScheduleMerge();
	// bool checkCompletionOfMergerRole();
	//static void activateWithOffsetAndMasterID(
		//	int offset,
			//int MasterID);
	static int timeOfNextMergeWake();

private:
	static void mergeMyClique(Message msg);
	static void mergeOtherClique(Message msg);
};
