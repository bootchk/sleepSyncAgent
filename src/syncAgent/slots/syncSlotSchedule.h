
#include "../types.h"
#include "../../augment/timeMath.h"

class SyncSlotSchedule {
public:
	static DeltaTime deltaToThisSyncSlotMiddleSubslot();
	static DeltaTime deltaToThisSyncSlotEnd();

	static LongTime timeOfThisSyncSlotMiddleSubslot();
	static LongTime timeOfThisSyncSlotEnd();	// Of this period
};
