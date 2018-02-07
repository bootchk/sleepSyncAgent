
#pragma once

#include "messageHandler.h"



/*
 * Responsibility:
 * - filter packets for CRC errors
 * - serialize to messages
 * - filter received messages
 * - dispatch to handler by slot type
 */

class MessageDispatcher {
public:
	static HandlingResult dispatch(MessageHandler msgHandler);
};
