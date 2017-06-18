
#pragma once


/*
 * Keep state of syncing:
 * - syncing: actively listening and sending for sync (adjusting for drift.)
 * - maintaining: just counting out syncPeriods (which will drift)
 */
class SyncState {
public:
	static void setActive();
	static void setPaused();

	static bool shouldAbandonMastership();
};
