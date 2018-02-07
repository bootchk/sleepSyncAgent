#pragma once

#include "../message/message.h"



/*
 * Knows diameter of clique i.e. granularity of hopped network.
 *
 * Responsibilities:
 *  - accept provisioning (control) of granularity
 *  - filter messages by  granularity of network (virtual range)
 *  - set xmit power according to granularity of network
 */
class NetworkTopology {

private:
	/*
	 * Signatures: ProvisionCallback
	 *
	 * Called from publisher, on notify
	 */
	static void handleNetGranularityProvisioning(uint32_t);
	static void handleScatterProvisioning(uint32_t);


public:

	static void subscribeToProvisionings();

	static void handleNetGranularityMessage(SyncMessage* msg);
	static void handleScatterMessage(SyncMessage* msg);
};
