
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
	static void bookkeepingAtPreludeToSyncSlot();


	static void dispatchSyncSlotKind();

	static void beginListen();
	static void endListen();
};
