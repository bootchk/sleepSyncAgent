
#include <timeMath.h>

#include "../../types.h"


/*
 * Schedule for FishSlot
 *
 * Determines:
 * - what slot to fish in
 * - slot duration
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
