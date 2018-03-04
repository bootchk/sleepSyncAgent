
#pragma once

#include "provisioningPublisher.h"


/*
 * Knows how provisioning work via BT protocol.
 *
 * Like network provisioner.
 * Is top half of provisioning.
 *
 * See workProvisioningProxy for bottom half.
 */

class WorkProvisioner {
public:
	static void subscribeToProvisionings();

	static void setConverterFunc(ConverterFunc aConverterFunc);
};
