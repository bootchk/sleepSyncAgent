
#pragma once


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
	 * actively syncing and also fishing for other cliques and merging them
	 * Two slots active.
	 */
	SyncAndFishMerge
};


class SyncModeManager {
public:
	static SyncMode mode();

	/*
	 * Check power levels and possibly transition to other modes.
	 */
	static void tryTransitions();
};
