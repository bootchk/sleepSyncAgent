
#include <timeMath.h>

#include "../../types.h"


/*
 * Schedule: returns times and deltas
 * For Fishing
 * Knows start and end times of Fish slot
 *
 * Initialized each sync period, before fishing, if role isFisher and SyncMode includes fishing
 *
 * Fishing may be aligned with slots (Trolling), or not (DeepFishing.)
 *
 * Collaborates with Schedule to ensure times are within sync period
 */
class FishSchedule {
public:
	static void initStartTime();


	static DeltaTime deltaToSlotStart();
	static DeltaTime deltaToSlotEnd();

	static LongTime timeOfThisFishSlotEnd();

private:
	static void memoizeTimeOfThisFishSlotStart();

};
