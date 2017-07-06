
#pragma once

#include "../messageHandler/messageHandler.h"


/*
 * Understands sleeping for sync algorithm.
 *
 * Responsibilities:
 * - sleep until a timeout
 * - sleep until either a timeout or valid message received and accepted
 * - filter invalid message packets received
 *
 * A main concern is that the platform Sleeper may wake unexpectedly on events.
 * This code guarantees that timeout has elapsed, even in face of unexpected wake.
 *
 * You can call init() more than once, with different maxSaneTimeout limits.
 */

typedef void (*MsgReceivedCallback)();



class SyncSleeper {

public:

	// OLD static void clearReasonForWake();

	/*
	 * Sleep a given time (a func that calculates timeout, or a constant timeout)
	 * Expect only a timer event, but allow for unexpected waking events.
	 */
	static void sleepUntilTimeout(TimeoutFunc);
	// OLD static void sleepUntilTimeout(DeltaTime);

	/*
	 * Expect either a timeout or a message event.
	 */
	static HandlingResult sleepUntilMsgAcceptedOrTimeout(
			MessageHandler,
			TimeoutFunc);

	static MsgReceivedCallback getMsgReceivedCallback();

	// debugging
	static unsigned int getCountSleeps();
	static unsigned int getPriorReasonForWake();
};
