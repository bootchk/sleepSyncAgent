
#include "syncPowerManager.h"


/*
 * Choose a set of levels corresponding to solar cell Voc
 * or for other testing reasons.
 *
 * True:  Voc == 2.7V @ 1000 lux e.g. AM1454
 * False: Voc == 3.6V @ 1000 lux e.g. Powerfilm LL3-37
 *
 * For low votage, require larger storage capacitor to contain same energy
 * (energy depends on voltage drop from storage capacitor.)
 */
#define LOW_VOLTAGE_SOLAR 1




void SyncPowerManager::init() {
	// Delegate to generic PowerManager
	PowerManager::init();
}

void SyncPowerManager::enterBrownoutDetectMode() { PowerManager::enterBrownoutDetectMode(); }

VoltageRange SyncPowerManager::getVoltageRange() { return PowerManager::getVoltageRange(); }

// This depends on the radio chip, not the solar cell
bool SyncPowerManager::isPowerExcess() { return PowerManager::isPowerExcess(); }


/*
 * For some debugging: bool SyncPowerManager::isPowerForSync()  { return PowerManager::isPowerAboveUltraHigh(); }	// > 3.2
 * For some debugging with 2AAA batteries @2.68V
 */


#ifdef LOW_VOLTAGE_SOLAR

// Require more to start loop
bool SyncPowerManager::isPowerForStartLoop() { return PowerManager::isPowerAboveMedium(); }	// > 2.5

// Require more to work or fish
bool SyncPowerManager::isPowerForWork()      { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForFishMode()  { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForFishSlot()  { return PowerManager::isPowerAboveMedium(); } // > 2.5

// Require less to maintain sync
bool SyncPowerManager::isPowerForSyncMode()  { return PowerManager::isPowerAboveLow(); } // > 2.3
bool SyncPowerManager::isPowerForSyncSlot()  { return PowerManager::isPowerAboveLow(); } // > 2.3

// From 1.7 (BOR) to 2.3, keep sync, but not maintain

#else

bool SyncPowerManager::isPowerForWork()      { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForStartLoop() { return PowerManager::isPowerAboveHigh(); }	// > 2.7
bool SyncPowerManager::isPowerForSyncMode()  { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForFishMode()  { return PowerManager::isPowerAboveHigh(); }	// > 2.7
bool SyncPowerManager::isPowerForFishSlot()  { return PowerManager::isPowerAboveMedium(); } // > 2.5
bool SyncPowerManager::isPowerForSyncSlot()  { return PowerManager::isPowerAboveLow(); } // > 2.3

#endif



