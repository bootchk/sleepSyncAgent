
#pragma once

#include "../message/message.h"


typedef void (*WorkControlCallback)(uint32_t);

/*
 * Knows how to control an app's work.
 *
 * Responsibility:
 *
 * callback app to provision work
 *
 * subscribe to provisioning
 *
 * enact stages of provisioning
 */

class WorkControlProxy {
public:
	/*
	 * API from app to SleepSync
	 */
	static void setWorkTimeControlCallback(WorkControlCallback);
	static void setWorkCycleControlCallback(WorkControlCallback);
	static WorkControlCallback getWorkTimeControlCallback();
	static WorkControlCallback getWorkCycleControlCallback();

	/*
	 * API from SleepSync to app
	 */
	static void handleWorkTimeMessage(SyncMessage* msg);
	static void handleWorkCycleMessage(SyncMessage* msg);
};


/*
 * Not used design: provisioning include an offset, which sets WorkTime
 *
 * WorkTime::setPeriodTimeToWork
 */
