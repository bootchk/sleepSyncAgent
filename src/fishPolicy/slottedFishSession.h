#pragma once

#include "../types.h"	// DeltaTime


/*
 * Variable duration fishing session.
 * In units of slot.
 * Each session starts on a slot boundary.
 * Actual duration may be truncated to avoid following sync slot.
 */
class SlottedFishSession {
public:
	/*
	 * Allow caller to set duration in ticks.
	 * So duration might not be same duration as virtual slot.
	 */
	static void incrementFishSessionDuration(unsigned int increment);
	static void decrementFishSessionDuration(unsigned int decrement);
	/*
	 * One slot.  SyncAgent will quit fishing altogether if not enough power.
	 */
	static void setDurationToMinDuration();

	// Duration in units ticks
	static DeltaTime duration();
	// Duration in units slot
	static unsigned int slotDuration();
};