
#include "../types.h"
#include "../../augment/timeMath.h"

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
	static LongTime timeOfThisFishSlotStart();

};
