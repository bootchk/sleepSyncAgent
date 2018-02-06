#pragma once

#include <platformTypes.h>

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
	 */
	static void handleCliqueSizeProvisioning(uint32_t);
	static void handleScatterProvisioning(uint32_t);


public:

	static void subscribeToProvisionings();

	/*
	 * Effectively reducing size of cliques,
	 * since reduces range of master's messages.
	 * !!! Cliques may lose and seek new masters.
	 */
	//static void setXmitPower(WorkPayload);

	static bool isMsgInVirtualRange(int8_t rssi);

	static void setGranularity(SyncMessage* msg);
};
