
#include "../../syncAgentImp/state/syncMode.h"

#include "../../modules/syncPowerManager.h"
#include "../../logging/logger.h"
#include "../../policy/provisionManager.h"

// Actions for transitions
#include "../../syncAgentImp/state/role.h"
#include "../../syncAgentImp/syncAgentImp.h"


namespace {

SyncMode _mode = SyncMode::Maintain;

#ifdef FUTURE
/*
 * Are we paused for long time and are we master?
 */
void pauseSync() {
	if ( SyncState::shouldAbandonMastership() ) {
			// doAbandonMastershipSyncPeriod
			// For now, no abandon mastership i.e. we never get here
			assert(false);
	}
}
#endif


void toSyncOnly() {
	_mode = SyncMode::SyncOnly;
	SyncAgentImp::ToNoFishingFromOther();
}

#ifdef BLE_PROVISIONED
void toSyncAndProvision() {
	_mode = SyncMode::SyncAndProvision;
}
#endif


}


void SyncModeManager::resetToModeMaintain() {
	_mode = SyncMode::Maintain;

	// sub mode
	MergerFisherRole::toNoFishing();
}


SyncMode SyncModeManager::mode() { return _mode; }


/*
 * Implementation notes:
 *
 * State diagram is linear:  Maintain<=>SyncOnly<=>SyncAndFishMerge
 * There are no transitions directly from low to high mode, must step up and down through modes.
 */
void SyncModeManager::checkPowerAndTryModeTransitions(){
	switch(_mode) {
	case SyncMode::Maintain:
		/*
		 * Can't fall to lower mode: just browns out.
		 */
		if ( SyncPowerManager::isPowerForSyncMode() ) {
			// to next higher level
			// FUTURE onSyncingResumedCallback();	// Tell app
			Logger::logStartSync();
			_mode = SyncMode::SyncOnly;
		}
		// else no change to mode
		break;

	case SyncMode::SyncOnly:
		// to lower level?
		if ( ! SyncPowerManager::isPowerForSyncMode() ) {
			Logger::logPauseSync();
			_mode = SyncMode::Maintain;
		}
		else // to higher level?
			if ( SyncPowerManager::isPowerForFishMode() ) {
				Logger::logStartFish();
				_mode = SyncMode::SyncAndFishMerge;
				SyncAgentImp::toFisherFromNoFishing();
			}
		// else no change to mode
		break;

	case SyncMode::SyncAndFishMerge:
		/*
		 * Here, SyncAgentImp adjusts sub mode: MergerFisherRole
		 */

		// to lower level?
		if ( ! SyncPowerManager::isPowerForFishMode() ) {
			Logger::logStopFish();
			toSyncOnly();
		}
		else {
#ifdef BLE_PROVISIONED
		// TODO occasionally to SyncAndProvision
		if (ProvisionManager::shouldProvision()) {
			toSyncAndProvision();
		}
#else
		// no change to mode
#endif
		}
		break;
		// No higher level

	case SyncMode::SyncAndProvision:

		// We only stay in this mode for one sync period
		// TODO this is hacky, should be a policy class for this
		if ( ! SyncPowerManager::isPowerForFishMode() ) {
			toSyncOnly();
		}
		else {
			_mode = SyncMode::SyncAndFishMerge;
		}

		break;
		// XXX higher level is excess power, do more fishing or power shedding
	}

}
