
#pragma once

#include <inttypes.h>


/*
 * Knows how to provision network topology.
 * Provisioning: control protocol using client app and BT.
 * Topology is changeable without provisioning, using SleepSync protocol.
 * Provisioning is the top half of the topology change stack.
 * See topology.h for the bottom half.
 *
 * Responsibilities:
 *  - accept provisioning (control) of granularity
 *
 *
 * Not built in build configs without BT provisioning and SD.
 */

class NetTopologyProvisioner {
	/*
	 * Signatures: ProvisionCallback
	 *
	 * Called from publisher, on notify
	 */
	static void handleNetGranularityProvisioning(uint32_t);
	static void handleScatterProvisioning(uint32_t);
public:
	static void subscribeToProvisionings();
};
