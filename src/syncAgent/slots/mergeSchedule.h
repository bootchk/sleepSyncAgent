
#include "../types.h"	// DeltaTime
#include "../../augment/timeMath.h"	// LongTime
#include "../modules/periodTime.h"

/*
 * Schedule for MergeSlot
 */
class MergeSchedule {
public:
	static DeltaTime deltaToThisMergeStart(const PeriodTime* const offset);	 // <<<<
	static LongTime timeOfThisMergeStart(DeltaTime offset);
};
