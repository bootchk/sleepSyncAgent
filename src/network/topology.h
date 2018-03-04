#pragma once

#include "../message/message.h"



/*
 * Manages changing of network topology:
 * - granularity of hopped network
 * - scatter
 *
 * Responsibilities:
 *  - filter messages by  granularity of network (virtual range)
 *  - set xmit power according to granularity of network
 *
 *  See also topologyProvisioner.h for top half.
 */
class NetworkTopology {
public:
	static void masterTellCliqueGranularityChange(NetGranularity granularity);
	static void masterTellCliqueToScatter();

	static void handleNetGranularityMessage(SyncMessage* msg);
	static void handleScatterMessage(SyncMessage* msg);
};
