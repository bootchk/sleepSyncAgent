
#pragma once

#include "syncSlotProperty.h"




/*
 * Supports SyncSlot tasks
 */
class SyncSlot {
public:
	static SyncSlotKind kind();

	static void dispatchSyncSlotKind();
	static void beginListen();
};
