
#include <cassert>
#include "../modules/schedule.h"


// FUTURE, resettable and a policy that fishes outward in both directions from sync slot.
/*
 * Decides sequence of normally sleeping slot to fish in.
 *
 * Circular.
 * In sequential order.
 */
class FishPolicy {
private:
	ScheduleCount counter = Schedule::FirstSleepingSlotOrdinal;
public:
	/*
	 * Returns ordinal, i.e. not a zero-based index.
	 *
	 * Note this implementation yields: 3,4,...,CountSlots,2,3,4...
	 * i.e. after a reset it does not start at the first sleeping slot
	 */
	ScheduleCount next() {
		ScheduleCount result;

		counter++;
		if (counter > Schedule::CountSlots) {
			counter = Schedule::FirstSleepingSlotOrdinal;
		}
		result = counter;
		assert(result >= Schedule::FirstSleepingSlotOrdinal && result <= Schedule::CountSlots);
		return result;
	}
};
