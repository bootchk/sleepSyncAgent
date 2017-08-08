
#include "syncMode.h"

#include "role.h"
#include "../modules/syncPowerManager.h"
#include "../logging/logger.h"

// Actions for transitions
#include "../syncAgent.h"


namespace {

SyncMode _mode;

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

}


void SyncModeManager::init() {
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
void SyncModeManager::tryTransitions(){
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
				SyncAgent::toFisherFromNoFishing();
			}
		// else no change to mode
		break;

	case SyncMode::SyncAndFishMerge:
		/*
		 * Here, SyncAgent adjusts sub mode: MergerFisherRole
		 */

		// to lower level?
		if ( ! SyncPowerManager::isPowerForFishMode() ) {
			Logger::logStopFish();
			_mode = SyncMode::SyncOnly;
			SyncAgent::ToNoFishingFromOther();
		}
		// else no change to mode
		break;
		// No higher level
#ifdef FUTURE
		higher level is excess power, more fishing or power shedding
		// TODO
#endif
	}

}
