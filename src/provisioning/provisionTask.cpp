
// Part of SSTask class implementation, specific to <provisioning>
#include "task.h"

#include "../logging/logger.h"

// libBLEProvisionee
#include <provisioner.h>
#include "../syncAgentImp/syncAgentImp.h"
#include "../schedule/syncSchedule.h"

// assertions
#include "../schedule/radioPrelude.h"
#include <cassert>


void SSTask::provisionStart() {
	// assert Provisioner is Init

	// We left HFXO running
	assert(RadioPrelude::isDone());

	APIError result = Provisioner::start();
	if (result == APIError::BLEStartedOK ) {
		/*
		 * Softdevice is in charge.
		 * Sleep (use sd_app_evt_wait()) until Provisioner callback.
		 * IE continue in main loop on WFI.
		 * Assert callback will come in finite time (timeout, error, or succeed.)
		 */
	}
	else {
		/*
		 * Unrecoverable error.
		 * Schedule next task.
		 * We might try provision again, on the next iteration of Provisioning cycle.
		 */
		Logger::log("SD error at Provisioner::start");
		SSTask::provisionEnd();
	}
}

void SSTask::provisionEnd() {
	Provisioner::shutdown();

	// Provisioner configured radio for BT, now reconfigure for SleepSync protocol.
	SyncAgentImp::initEnsembleProtocol();

	/*
	 * Now Softdevice is not active and not generating events.
	 * Schedule the next task.
	 * Prelude might still be done (HFXO running.)
	 */
	SyncSchedule::syncSlotAfterProvisioning();

	// shutdown() already assert(not Provisioner::isProvisioning());
}


#ifdef NOT_USED
namespace {

void logProvisioningError(ProvisioningResult result) {
	switch(result) {
	case ProvisioningResult::SDError:
	case ProvisioningResult::SDErrorOnSDEnable:
	case ProvisioningResult::SDErrorOnBLEConfig:
	case ProvisioningResult::SDErrorOnBLEEnable:
	  Logger::log("SD error during provisioning"); break;
	default:
		// OK
		break;
	}
}
}  // namespace
#endif

