
#include <timeMath.h>

#include "../types.h"


/*
 * Schedule for FishSlot
 *
 *
 */
class FishSchedule {
public:
	static void init();

	static DeltaTime deltaToSlotStart();
	static DeltaTime deltaToSlotEnd();

	static LongTime timeOfThisFishSlotEnd();

private:
	static void memoizeTimeOfThisFishSlotStart();

};
