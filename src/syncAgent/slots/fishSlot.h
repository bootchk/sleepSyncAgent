
#pragma once

#include "../modules/message.h"


class FishSlot {
public:
	static void perform();
private:

	static bool dispatchMsgReceived(SyncMessage* msg);
	static void doMasterSyncMsg(SyncMessage* msg);
};
