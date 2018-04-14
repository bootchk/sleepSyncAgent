
#pragma once


/*
 * Understands the radio prelude: starting HFXO.
 *
 * Knows when it needs to be done and undone.
 *
 * Issues:
 * - it takes time to do
 * - and wastes energy if done and not needed
 *
 * It is better to leave it done if the radio will be used again soon.
 *
 * The "takes time" issue is that since a slot uses the radio,
 * the prelude must be done in advance of a slot.
 * If there is insufficient time before the next slot, the prelude should be left done.
 */
/*
 * Collaborates with FishingManager to know the completed or to-be-done fish slot
 */
class RadioPrelude {
public:
	static void doIt();
	static void undo();
	/*
	 * Only whether doIt() was called, not whether prelude finite time elapsed.
	 */
	static bool isDone();

	/*
	 * If appropriate for next slot, undo.
	 * Returns true if RadioPrelude was undone
	 */
	static bool tryUndoAfterSyncing();
	static bool tryUndoAfterFishing();
	static bool tryUndoAfterMerging();
	static bool tryUndoAfterProvisioning();


private:
	/*
	 * Whether the next slot is soon enough to merit leave prelude done.
	 */
	static bool shouldUndoAfterFishing();
	static bool shouldUndoAfterSyncing();
	static bool shouldUndoAfterMerging();
	static bool shouldUndoAfterProvisioning();
};
