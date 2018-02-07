
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
 */
class Granularity {
public:
	/*
		 * Effectively reducing size of cliques,
		 * since reduces range of master's messages.
		 * !!! Cliques may lose and seek new masters.
		 */
		//static void setXmitPower(WorkPayload);
	static void setGranularity(NetGranularity granularity);

	static bool isMsgInVirtualRange(int8_t rssi);
		// TODO caller of this
};
