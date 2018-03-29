#pragma once

#include "../types.h"	// DeltaTime


/*
 * Knows parameters of variable duration fishing session.
 * In units of slot.
 * Each session starts on a slot boundary.
 *
 * Responsibilities (Collaborator):
 * - Set parameters (App power mgt)
 * - Get parameters (FishPolicy)
 * - Reset fish policy when parameters change (FishPolicy)
 *
 * !!! FishPolicy knows properties of the current session, based on these parameters.
 * SlottedFishSession does not know current fish session.
 *
 * Current fish session may be truncated to avoid sync slot, that is decided in FishPolicy
 */
class SlottedFishSession {
public:
	/*
	 * Allow caller to set duration in ticks.
	 * So duration might not be same duration as virtual slot.
	 */
	static void incrementFishSessionDuration(SlotCount increment);
	static void decrementFishSessionDuration(SlotCount decrement);
	/*
	 * One slot.  SyncAgent will quit fishing altogether if not enough power.
	 */
	static void setDurationToMinDuration();


	static DeltaTime durationInTicks();
	static SlotCount durationInSlots();
};
