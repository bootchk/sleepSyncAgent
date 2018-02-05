/*
 * Part of FishSlot implementation
 */
#include "fishSlot.h"

#include "../../clique/clique.h"
#include "../../globals.h"

#include "../../logging/logger.h"


/*
 * Callbacks
 *
 * These are part of state for DeepFishing.
 * When start DeepFishing, choose one of these callbacks.
 */


void FishSlot::endDeepFishingWithNoAction() {
	/*
	 * Self stays in current clique.
	 *
	 * (a slave merged away to a superior OR a superior fished us)
	 * AND we didn't hear the superior
	 *
	 * Self could be master.
	 */
	// FUTURE remember so we aren't continually fished to another.
	Logger::log("Deep done, no action.\n");
}

// TODO wording merged away => departed
void FishSlot::endDeepFishingWithRecoverMaster() {
	/*
	 * Master merged away to superior
	 * AND we didn't hear a superior.
	 * We were a slave.
	 * Our clique's masterID is still the departed master.
	 *
	 * Another similar slave could have finished deep fishing ahead of us
	 * and assumed mastership.
	 * In that case, we might have heard Sync from them in sync slot.
	 * Recover master if we haven't already heard another assuming mastership.
	 * TODO this is fuzzy
	 */
	/*
	 * For now, many slaves may assume Mastership
	 */
	// FUTURE: bettery strategy
	Logger::log("Deep done, recover master.\n");
	clique.assumeMastership();
}
