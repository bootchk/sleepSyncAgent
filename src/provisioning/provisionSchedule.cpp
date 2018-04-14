
#include "provisionSchedule.h"





DeltaTime ProvisionSchedule::deltaToProvisionStart() {
	/*
	 * Could start anytime in normally sleeping slots.
	 *
	 * If start now (while HFXO is still on) might save time/energy in the SD.
	 * But it could interfere with other sync slots slightly drifted?
	 */
	return 0;
}


DeltaTime ProvisionSchedule::deltaToProvisionEnd() {
	/*
	 *
	 */
	// TODO Task
	return 2000;
}
