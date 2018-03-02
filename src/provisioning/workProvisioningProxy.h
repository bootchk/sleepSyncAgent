
#pragma once


#include "provisioningPublisher.h"   // ProvisionCallback


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
	static void setWorkTimeFinalProvisioningCallback(ProvisionCallback);
	static void setWorkCycleFinalProvisioningCallback(ProvisionCallback);

	static void subscribeToProvisionings();
};


/*
 * Not used design: provisioning include an offset, which sets WorkTime
 *
 * WorkTime::setPeriodTimeToWork
 */
