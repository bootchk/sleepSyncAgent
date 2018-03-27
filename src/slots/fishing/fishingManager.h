#pragma once

#include "fishingMode.h"

#include <clock/longClock.h>  // LongTime
#include "../../modules/deltaSync.h"  // DeltaSync




/*
 * Manages fishing modally according o current FishingMode
 *
 * Knows:
 * - current FishingMode
 * - schedule of FishSession by mode
 * - episodic nature of fishing by mode
 *
 * Switches mode on commmand, understands constraints.
 */
/*
 * Algebra: legal sequences:
 *
 * - Usual sequence
 * switchToTrolling(); getStartTimeToFish(); checkFishingDone();....getStartTimeToFish();
 *
 * - mode can be switched regardless of current mode.
 * 	switchToTrolling();switchToTrolling();
 * 	switchToDeepFishing();switchToDeepFishing();
 *
 * - mode can be restarted even if mode is not current mode.  Does not switch to mode.
 * switchToDeepFishing(); restartTrollingMode();
 *
 * - every mode can checkFishingDone
 * switchToDeepFishing(); checkFishingDone(); switchToTrolling(); checkFishingDone();
 *
 *
 */
class FishingManager {
public:
	static FishingMode mode();

	/*
	 * Mark that fishing was done.
	 * Proceeds to another spot if trolling.
	 * Possible change mode, Deep=>Trolling when Deep is done
	 *
	 * Deep is episodic and finite.
	 * Check for completion, call callback for event, and switch mode to Trolling.
	 */
	static void checkFishingDone();




	/*
	 * FishingMode
	 */
	static void switchToTrolling();
	static void switchToDeepFishing(DeltaTime, Callback);
	/*
	 * Restart trolling policy.
	 *
	 * We may already be deep fishing for a new master.
	 * This does NOT force mode to trolling, deep fishing might continue.
	 *
	 * Called when we suspect a master would probably be found if policy restarts fishing around sync slot.
	 * E.G. when master dropped out
	 */
	static void restartTrollingMode();



	/*
	 * FishSession schedule by current mode.
	 *
	 * Currently result is "wild": just what we were told by another unit via Merge msg.
	 * Caller must ensure it meets other constraints.
	 */
	static LongTime getStartTimeToFish();
	static DeltaTime getFishSessionDuration();




	static bool isFishSlotStartSyncPeriod();
	static bool isFishSlotEndSyncPeriod();
};
