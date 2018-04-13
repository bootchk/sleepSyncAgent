
#pragma once

/*
 * Steps (called from a task) for sync slot.
 *
 * TODO make SyncSlotKind a polymorphic class
 */
class SyncSlotStep {
public:
	static void sendWorkSync();
	static void sendSync();
	static void sendControlSync();
};
