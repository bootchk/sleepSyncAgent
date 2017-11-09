#pragma once


#include <radioSoC.h>  // LongTime
#include "../../modules/deltaSync.h"  // DeltaSync




/*
 * FishingMode is (roughly speaking) a subclass of MergerFisherRole::Fisher.
 * Each FishingMode is associated with a FishingPolicy
 *
 * Trolling continues indefinitely, wrapping around.
 * DeepFishing is finite, and generates an event on completion.
 *
 * Caller knows when fishing succeeds (fish another clique's Master.)
 * Generally that entails a switch to another FishingMode.
 *
 * When DeepFishing, another request to switchDeepFishing() might be equivalent,
 * and does not restart DeepFishing.
 * Otherwise, it restarts DeepFishing(internally)
 *
 */
enum class FishingMode {
	Trolling,	// Scanning across sync period, by slots.
	DeepFishing	// Repetitively fishing one sub-period of sync period (not necessarily a slot.)
};


/*
 * Manages current FishingMode
 *
 * Knows current FishingMode
 * Switches mode on commmand, understands constraints.
 * Knows start time to fish
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
	/*
	 * If current mode is finite, check for completion and call callback for event.
	 */
	static void checkFishingDone();

	static void switchToTrolling();
	static void switchToDeepFishing(DeltaTime, Callback);

	/*
	 * Get from current mode.
	 *
	 * Currently result is "wild": just what we were told by another unit via Merge msg.
	 * Caller must ensure it meets other constraints.
	 */
	static LongTime getStartTimeToFish();

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
};
