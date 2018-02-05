
#include <timeMath.h>


/*
 * Knows schedule within sync slot ie when middle and end
 */
class SyncSlotSchedule {
public:
	/*
	 * Start time of the middle subslot of the SyncSlot.
	 * Transmit occurs in the middle subslot.
	 * We want the transmit to be in the middle of the subslot.
	 * Since there is a ramp up delay, start the subslot before the middle of the SyncSlot.
	 */
	static DeltaTime deltaToThisSyncSlotMiddleSubslot();
	static DeltaTime deltaToThisSyncSlotEnd();

	static LongTime timeOfThisSyncSlotMiddleSubslot();
	static LongTime timeOfThisSyncSlotEnd();	// Of this period
};
