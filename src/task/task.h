
#pragma once

class SSTask {
public:
	static void radioPrelude();

	static void startSyncSlotAfterPrelude();

	static void startSyncSlotWithoutPrelude();

	// initiate send by syncSendKind (fork task on radio peripheral)
	static void sendSync();

	static void endSyncSlotListen();
	static void endSyncSlotSend();

	static void startSyncPeriodMaintain();
	static void startSyncSlotWithoutScheduledPrelude();

	static void tryFishOrMerge();
	static void scheduleFishSlot();
	static void fishSlotStart();
	static void fishSlotEnd();

	static void provisionStart();
	static void provisionEnd();

	static void mergerStart();
	static void mergerEnd();
};
