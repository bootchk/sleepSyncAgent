
#pragma once

#include "granularity.h"
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
	/*
	 * If we have fulfilled need to send, perform action.
	 */
	static void checkDoneAndEnactControl();


	static MessageType currentMsgType();
	static WorkPayload currentPayload();
};
