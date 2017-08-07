
#include "syncMode.h"

#include "../modules/syncPowerManager.h"
#include "../logging/logger.h"

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


SyncMode SyncModeManager::mode() {
	return _mode;
}


void SyncModeManager::tryTransitions(){
	switch(_mode) {
	case SyncMode::Maintain:
		/*
		 * Can't fall to lower mode: just browns out.
		 */
		if ( SyncPowerManager::isPowerForSync() ) {
			// to next higher level
			// FUTURE onSyncingResumedCallback();	// Tell app
			Logger::logStartSync();
			_mode = SyncMode::SyncOnly;
		}
		break;
	case SyncMode::SyncOnly:
		// to lower level?
		if ( ! SyncPowerManager::isPowerForSync() ) {
			Logger::logPauseSync();
			_mode = SyncMode::Maintain;
		}
		else // to higher level?
			if ( SyncPowerManager::isPowerForFish() ) {
			//Logger::logPauseSync();
			_mode = SyncMode::SyncAndFishMerge;
			// TODO
		}

		break;
	case SyncMode::SyncAndFishMerge:
		// to lower level?
		if ( ! SyncPowerManager::isPowerForFish() ) {
			//Logger::logStopFish();
			_mode = SyncMode::SyncOnly;
		}
		// No higher level
#ifdef FUTURE
		higher level is excess power, more fishing or power shedding
		else // to higher level?
			if ( SyncPowerManager::isPowerForFish() ) {
				//Logger::logPauseSync();
				_mode = SyncMode::SyncAndFishMerge;
				// TODO
			}
		break;
#endif
	}

}
