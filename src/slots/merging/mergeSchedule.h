
#include <timeMath.h>

#include "../../clique/periodTime.h"

/*
 * Schedule for MergeSlot
 */
class MergeSchedule {
private:
	static LongTime timeOfThisMergeStart(DeltaTime offset);
	static DeltaTime deltaToThisMergeStart(const PeriodTime* const offset);

public:
	/*
	 * DeltaTime from now to time to start MergeSlot
	 */
	static DeltaTime deltaToThisMergeStart();

	static bool isMergerStartSyncPeriod();
	static bool isMergerEndSyncPeriod();
};
