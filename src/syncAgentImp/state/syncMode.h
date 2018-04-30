
#pragma once

// TODO rename Maintain=>Marking
enum class SyncMode {
	/*
	 * just counting out syncPeriods (which will drift).
	 * No slots active (not using radio).
	 */
	Maintain,
	/*
	 * actively listening and sending for sync (adjusting for drift.)
	 * One slot active.
	 */
	SyncOnly,
	/*
	 * actively syncing and also fishing for other cliques and merging them.
	 * Two slots active.
	 */
	SyncAndFishMerge,
	/*
	 * Sync slot and a period (not slotted) for provision.
	 */
	SyncAndProvision
};


class SyncModeManager {
public:
	/*
	 * To least power-hungry, least processing mode.
	 * Every loop tries to transition from this state.
	 */
	static void resetToModeMaintain();

	static SyncMode mode();

	static void checkPowerAndTryModeTransitions();
};
