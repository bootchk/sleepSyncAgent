
#pragma once

#include "../message/message.h"

enum class NetGranularity {
	Small,
	Medium,
	Large
};


/*
 * Knows granularity.
 * A property of a clique.
 *
 * AKA diameter of cliques or subnetworks in a hopping/mesh network.
 *
 * Implemented by:
 * - reducing xmit power
 * - filtering messages on virtual range
 *
 * Any change may result in:
 * - Decrease: cliques lose master and seek new master.
 * - Increase: cliques merge
 */
class Granularity {
public:
	static void setGranularity(NetGranularity granularity);

	/*
	 * Is distance to sender within radius of my granularity.
	 */
	static bool isMsgInVirtualRange(
		unsigned int receivedSignalStrength,
		NetGranularity transmittedSignalStrength
		);
		// TODO caller of this
};
