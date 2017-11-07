#pragma once


#include <radioSoC.h>  // LongTime


enum class FishingMode {
	Trolling,	// Scanning across sync period, by slots.
	DeepFishing	// Fishing one sub-period of sync period (not necessarily a slot.)
};



class FishingManager {
public:

	static void switchToTrolling();
	static void switchToDeepFishing();

	static LongTime getStartTimeToFish();

	/*
	 * Restart trolling.
	 *
	 * We may already be deep fishing for a new master.
	 * This does NOT force mode to trolling, deep fishing might continue.
	 *
	 * Called when we lost sync?
	 * TODO
	 */
	static void restartTrollingMode();
};
