
#include <timeMath.h>

#include "../modules/periodTime.h"

/*
 * Schedule for MergeSlot
 */
class MergeSchedule {
public:
	/*
	 * DeltaTime remaining from time of call to time to start MergeSlot
	 */
	static DeltaTime deltaToThisMergeStart(const PeriodTime* const offset);	 // <<<<
	static LongTime timeOfThisMergeStart(DeltaTime offset);
};
