
#include <timeMath.h>

#include "../../types.h"


/*
 * Schedule: returns times and deltas
 * For Fishing
 * Knows start and end times of FishSession
 *
 * Initialized each sync period, before fishing, if role isFisher and SyncMode includes fishing
 *
 * Fishing may be aligned with slots (Trolling), or not (DeepFishing.)
 *
 * Collaborates with Schedule to ensure times are within sync period
 */
class FishSchedule {
public:
	// Calculate and remember start and end times
	static void setStartAndEndTimes();

	/*
	 * These may be called many times for each fish slot;
	 * because sleeper may be woken for other events.
	 */
	static DeltaTime deltaToFishSessionStart();
	static DeltaTime deltaToFishSessionEnd();

	static void logParameters();

private:
	static void memoTimeOfFishSessionStart();
	static void memoTimeOfFishSessionEnd();
	static LongTime timeOfFishSessionEnd();

};
