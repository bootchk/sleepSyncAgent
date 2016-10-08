
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
 */
 
 
 class MergePolicy {
	 static int countSendMergeSyncs;

 public:
	 static void restart();
	 static bool shouldScheduleMerge();
	 static bool checkCompletionOfMergerRole();
 };
