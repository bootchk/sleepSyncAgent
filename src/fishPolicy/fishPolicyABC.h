// Obsolete but some information value

/*
 * Generator of sequence of ordinal of normally sleeping slot to fish in.
 *
 * Abstract base class API:
 * - getStartTimeToFish()
 * - restart()
 * - checkDone()
 *
 * Subclasses:
 *
 * SimpleFishPolicy
 * - circular, ascending
 * - in sequential order
 * - restart() has no effect
 *
 * SyncRecoveryFishPolicy:
 * - expands outward from SyncPoint
 * - alternates direction, ascending and descending
 *
 * nextFishSlotOrdinal() returns ordinal, i.e. not a zero-based index.
 */


/*
 * Yields: 3,4,...,CountSlots,2,3,4...
 * i.e. after a reset it does not start at the first sleeping slot
 */
class SimpleFishPolicy {
public:
	SlotCount nextFishSlotOrdinal();
	void reset() {}	// Does nothing, generator continues as before
};
