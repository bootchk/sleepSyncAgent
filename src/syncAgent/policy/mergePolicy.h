
/*
 * Policy for xmitting MergeSync
 *
 * Invariant: Role.isMerger => MergePolicy is active
 *
 * Singleton.
 *
 * Responsibilities
 * 1. know circumstances for scheduling MergeSlot (and xmitting MergeSync)
 * 2. know when to quit role Merger (how long to continue Merging another clique.)
 *
 * MergePolicy knows to send MergeSync with collision avoidance.
 * A unit sends MergeSync probablistically to avoid contention.
 *
 * Many fishers may fish the same otherClique, assume role Merger, and contend to merge it.
 * The contention is with:
 * - master of otherClique (still sending Sync in its syncSlot)
 * - other mergers of otherClique
 *
 * Choices:
 * - a unit sends only one MergeSync, but with a slight random stagger from SyncPoint of otherClique.
 * - a unit repeatedly sends many MergeSyncs in a sequence of SyncPeriods.
 *
 * The chance that many Mergers will be contending depends on:
 * - the duty cycle
 * - count of units
 * - how early in the synch game it is
 *
 * FUTURE discuss each of these factors
 *
 * If master of otherClique fails to hear, they will keep mastering the other clique,
 * and slaves of otherClique that fail to hear will remain in the other clique.
 * If master of otherClique hears but slaves of otherClique fail to hear,
 * slaves will fallout of sync, and resume mastership of otherClique
 * ( possibly remain in their own single member clique.)
 * In both cases the clique(s) must be fished again.
 */
 
 
 class MergePolicy {
	 static int countSentMergeSyncs;

 public:
	 static void restart();
	 static bool shouldScheduleMerge();
	 static bool checkCompletionOfMergerRole();
 };
