
#include "provisionManager.h"

namespace {
	int counter = 0;
}


/*
 * SOFTDEVICE_PRESENT => provisioning capability
 * This might change, when BLE is used for more than provisioning.
 *
 * The provisioning manager is compiled even when not SOFTDEVICE_PRESENT,
 * so that less code is conditionally compiled.
 */
#ifdef SOFTDEVICE_PRESENT
/*
 * Provision periodically, regularly.
 *
 * Assert this is not in step with other units (in real time),
 * since each unit starts at a random real time.
 *
 * Also, any provisioning is randomized within a syncPeriod.
 * I.E. this won't collide with any other unit provisioning, with high probablity.
 *
 * IOW, no need to randomize this also, using RandomAlarmingClock.
 */
bool ProvisionManager::shouldProvision() {
	bool result = false;

	counter += 1;
	if (counter > 5) {
		counter = 0;
		result = true;
	}
	return result;
}
#else
bool ProvisionManager::shouldProvision() { return false; }
#endif
