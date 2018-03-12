
#include "syncPowerManager.h"


/*
 * These choose power levels.
 * Two factors:
 * - expected VocMax of solar cell
 * - whether prioritizing work over sync
 * - voltage reserve for starting sync algorithm
 *
 * EXPECTED VocMax:
 *
 * Choose a set of levels corresponding to solar cell Voc
 * or for other testing reasons.
 *
 * True:  Voc == 2.7V @ 1000 lux e.g. AM1454
 * False: Voc == 3.6V @ 1000 lux e.g. Powerfilm LL3-37
 *
 * For low votage, require larger storage capacitor to contain same energy
 * (energy depends on voltage drop from storage capacitor.)
 *
 * PRIORITIZING WORK
 *
 * We usually want to maintain sync with highest priority (lowest voltage).
 * Then we might choose between fishing and work.
 * Usually we want to make work the lowest priority, with fishing higher priority.
 */

// TODO make this dynamic instead of compile time




void SyncPowerManager::init() {
	// Delegate to generic PowerManager
	PowerManager::init();
}

void SyncPowerManager::enterBrownoutDetectMode() { PowerManager::enterBrownoutDetectMode(); }

VoltageRange SyncPowerManager::getVoltageRange() { return PowerManager::getVoltageRange(); }

// Vmax of radio chip, not the solar cell
bool SyncPowerManager::isPowerExcess() { return PowerManager::isPowerExcess(); }
bool SyncPowerManager::isPowerNearExcess() { return PowerManager::isPowerNearExcess(); }


/*
 * For some debugging: bool SyncPowerManager::isPowerForSync()  { return PowerManager::isPowerAboveUltraHigh(); }	// > 3.2
 * For some debugging with 2AAA batteries @2.68V
 */


// #ifdef LOW_VOLTAGE_SOLAR
// #define LOW_VOLTAGE_SOLAR 1

#define LOW_PRIORITY_WORK_Voc3_6	1


#ifdef LOW_PRIORITY_WORK_Voc3_6

/*
 * Sync prioritized over fishing over work
 */
bool SyncPowerManager::isPowerForStartLoop() { return PowerManager::isPowerAboveMedium(); }	// > 2.5

bool SyncPowerManager::isPowerForWork()      { return PowerManager::isPowerAboveHigh(); }	// > 2.7

bool SyncPowerManager::isPowerForFishMode()  { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForFishSlot()  { return PowerManager::isPowerAboveMedium(); } // > 2.5

bool SyncPowerManager::isPowerForSyncMode()  { return PowerManager::isPowerAboveUltraLow(); } // > 2.1
bool SyncPowerManager::isPowerForSyncSlot()  { return PowerManager::isPowerAboveUltraLow(); } // > 2.1

// From 1.7 (BOR) to 2.3, keep sync, but not maintain

#else

/*
 * For high Voc solar cells
 */
bool SyncPowerManager::isPowerForWork()      { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForStartLoop() { return PowerManager::isPowerAboveHigh(); }	// > 2.7
bool SyncPowerManager::isPowerForSyncMode()  { return PowerManager::isPowerAboveMedium(); }	// > 2.5
bool SyncPowerManager::isPowerForFishMode()  { return PowerManager::isPowerAboveHigh(); }	// > 2.7
bool SyncPowerManager::isPowerForFishSlot()  { return PowerManager::isPowerAboveMedium(); } // > 2.5
bool SyncPowerManager::isPowerForSyncSlot()  { return PowerManager::isPowerAboveLow(); } // > 2.3

#endif



