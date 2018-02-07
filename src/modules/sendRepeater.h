
#pragma once

#include "../message/message.h"



/*
 * Knows to repeat a message, with randomness so it is heard with high probability.
 *
 * In lieu of acknowledged messages.
 *
 * Receiver must handle duplicates.
 *
 * Hides a policy (how many times to repeat.)
 *
 * Remembers what message type to send
 */


// TODO see mergePolicy for similar

class SendRepeater {
public:
	static void start(MessageType, uint8_t value);
	static bool isActive();
	static bool shouldSend();
	static void stop();

	static MessageType currentMsgType();
	static WorkPayload currentPayload();
};
