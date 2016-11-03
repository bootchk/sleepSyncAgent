
#include "../modules/schedule.h"

/*
 * Decides sequence of normally sleeping slot to fish in.
 *
 * Circular.
 * In sequential order.
 */
class FishPolicy {
private:
	int counter = Schedule::FirstSleepingSlotOrdinal;
public:
	int next() {
		int result;

		counter++;
		if (counter > Schedule::CountSlots) {
			counter = Schedule::FirstSleepingSlotOrdinal;
		}
		result = counter;
		return result;	// ordinal between FirstSleepingSlotOrdinal and CountSlots
	}
};
