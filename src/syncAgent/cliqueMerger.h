
#include "../message.h"

/*
 * Record of a Clique merging into a better Clique.
 *
 * Invariant: Role.isMerger => CliqueMerger.isActive()
 *
 * Singleton.
 *
 * CliqueMerger knows how and when to schedule a mergerSlot.
 *
 * Note that many fishers may discover the same otherClique, try to merge it, and thus contend.
 *
 * Current implementation:
 *
 * A unit only sends one MergeSync.
 * If some members of otherClique fail to hear it, they will fallout of sync, and remain in their own (smaller?) clique.
 * And then they must be fished again.
 * If master of otherClique fails to hear, they will keep contending.
 * If slave fails to hear but master does hear, slave may fallout of sync and resume mastership of otherClique.
 *
 * A unit sends MergeSync probablistically, to avoid contention.
 * The contention is with:
 * - master of otherClique (still sending Sync in its syncSlot)
 * - other mergers of otherClique
 */
class CliqueMerger {

public:
	static void adjustBySyncMsg(Message msg);
	static bool isActive();
	static bool shouldScheduleMerge();
	// bool checkCompletionOfMergerRole();
	static void setOffsetAndMasterID();
	static int timeOfNextMergeWake();

private:
	static bool active;
	static int offsetToMergee;
	static int masterID;
	static int notifyCountdown;
};
