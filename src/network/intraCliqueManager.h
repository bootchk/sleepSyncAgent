
#pragma once

#include "netTypes.h"
#include "../message/message.h"

/*
 * Knows how to communicate to clique
 *
 * Primarily network control.
 */

class IntraCliqueManager {
public:
	/*
	 * Initiate upon provisioning.
	 */
	static void doUpstreamCliqueSizeChange(NetGranularity aGranularity);
	static void doDownstreamCliqueSizeChange(NetGranularity aGranularity);

	static void doUpstreamScatter();
	static void doDownstreamScatter();

	/*
	 * Are we sending control upstream or down?
	 */
	static bool isNeedSend();

	static MessageType currentMsgType();
	static WorkPayload currentPayload();
};
