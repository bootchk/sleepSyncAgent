
#pragma once

#include "../modules/message.h"

class WorkSlot {
public:
	static void perform();
private:
	static bool dispatchMsgReceived(SyncMessage* msg);
	static void start();
	static void end();


	static void doWorkMsg(WorkMessage* msg);
	static void xmitAproposWork();
	static void xmitWork();
};
