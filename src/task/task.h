
#pragma once

class SSTask {
public:
	static void radioPrelude();

	static void startSyncSlotAfterPrelude();

	static void startSyncSlotWithoutPrelude();

	static void sendSync();

	static void endListen();
	static void syncSlotEndSend();
};
