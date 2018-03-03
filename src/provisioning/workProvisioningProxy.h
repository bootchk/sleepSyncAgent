
#pragma once


#include "provisioningPublisher.h"   // ProvisionCallback

#include "../message/message.h"


/*
 * Knows how to provision an app's work.
 *
 * Responsibility:
 *
 * callback app to provision work
 *
 * subscribe to provisioning
 *
 * enact stages of provisioning
 */

class WorkProvisioningProxy {
public:
	/*
	 * API from app to SleepSync
	 */
	static void setWorkTimeFinalProvisioningCallback(ProvisionCallback);
	static void setWorkCycleFinalProvisioningCallback(ProvisionCallback);

	static void subscribeToProvisionings();

	static void setConverterFunc(ConverterFunc aConverterFunc);

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
