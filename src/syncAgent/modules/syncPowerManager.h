#pragma once

/*
 * Understands power levels required for SleepSync
 *  *
 * The levels in descending order of power required:
 * - work
 * - sync keeping (radio and clock)
 * - sync maintenance (clock only)
 *
 *
 * For example all the above are duty-cycled (bursts of):
 * - work:  100 mA motor or 20mA LED
 * - sync keeping:  4mA radi0
 * - sync maintenance: mcu
 * The base is the RTC of 1.5uA, which always runs to burst the mcu, etc.
 * If there is not enough power to maintain Vmin for the mcu, brownout and reset occurs.
 *
 * Boot requires a small reserve.
 * Radio requires about 0.2V on capacitor.
 * Radio and work must not brownout mcu.
 * Work requires > 0.2V on capacitor.
 * Radio is required to keep sync.
 * Sync is maintained but drifts if no power for radio.
 * Work may temporarily take Vcc below needed for radio.
 *
 * Practically, during development, you can change the levels here,
 * but only using pre-defined levels defined by PowerManager.
 * You can also change the pre-defined levels in the PowerManager.
 * Also, you can change behaviour by changing where these are referenced.
 * Finally, these might depend on the platform, where on the nRF52 more levels are pre-defined.
 * Some of the comments may be stale, don't trust them.
 */
#include <radioSoC.h>	// PowerManager

/*
 * Pure class
 */

class SyncPowerManager {

public:
	// Init generic PowerManager
	static void init() { PowerManager::init(); }

	static void enterBrownoutDetectMode() { PowerManager::enterBrownoutDetectMode(); }
	/*
	 * Levels
	 */
	// Above Vmax of chip 3.6V
	static bool isPowerExcess() { return PowerManager::isPowerExcess(); }

	// In sync and can work
	// static bool isPowerForWork() { return PowerManager::isPowerAboveHigh(); }	// > 2.7
	static bool isPowerForWork() { return PowerManager::isPowerAboveMedium(); }	// > 2.5

	/*
	 * Enough to begin the sync loop.
	 * Might require much to get past cpu usage during initialization.
	 *
	 * For some debugging: static bool isPowerForSync()  { return PowerManager::isPowerAboveUltraHigh(); }	// > 3.2
	 * For some debugging with 2AAA batteries @2.68V
	 */
	static bool isPowerForStartLoop()  { return PowerManager::isPowerAboveHigh(); }	// > 2.5

	/*
	 * In ascending order, corresponding to modes.
	 */
	// Enough to do a SyncWorkSlot
	static bool isPowerForSyncMode()  { return PowerManager::isPowerAboveMedium(); }	// > 2.5

	// Enough at beginning of loop to set mode to fish and merge
	static bool isPowerForFishMode()  { return PowerManager::isPowerAboveHigh(); }	// > 2.7


	/*
	 * Even though we power might have been adequate when we decided mode, it might have been exhausted.
	 * By ensemble startup? Or simple failing power?
	 */
	// Enough power to do a fish slot
	// Since sync slot was also done, this allows 0.2V consumed by sync slot
	static bool isPowerForFishSlot()  { return PowerManager::isPowerAboveMedium(); } // > 2.5

	// Enough to do a SyncSlot or portions of it
	static bool isPowerForSyncSlot()  { return PowerManager::isPowerAboveLow(); } // > 2.3

	// Only enough to count out SyncPeriods, not use radio
	// NOT USED: static bool isPowerForIdle()  { return PowerManager::isPowerAboveLow(); }	// > 2.3

	/*
	 * Ranges
	 */
	static VoltageRange getVoltageRange() { return PowerManager::getVoltageRange(); }
};
