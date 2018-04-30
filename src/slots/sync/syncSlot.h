
#pragma once

#include "syncSlotProperty.h"	// SyncSlotKind




/*
 * Supports SyncSlot tasks
 */
class SyncSlot {
public:
	static SyncSlotKind kind();

	/*
	 * For regular, or sync maintaining sync slot.
	 */
	static void bookkeepingAtStartSyncSlot();

	/*
	 * Called during RadioPrelude while waiting for HFXO to start.
	 * !!! Not called every syncPeriod (when RadioPrelude already done, i.e. fishing near sync slot.)
	 *
	 * Primary task is to check power,
	 * which might be exhausted by fishing or provisioning.
	 */
	static void bookkeepingAtPreludeToSyncSlot();


	static void dispatchSyncSlotKind();

	static void beginListen();
	static void endListen();
};
