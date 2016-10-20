
#pragma once

#include "../modules/message.h"


class FishSlot {
public:
	static void perform();
private:
	static void start();
	static void end();

	static bool dispatchMsgReceived(SyncMessage* msg);
	static void doMasterSyncMsg(SyncMessage* msg);
};
