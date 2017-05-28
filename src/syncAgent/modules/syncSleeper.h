
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

typedef void (*msgReceivedCallback)();



class SyncSleeper {

public:
	/*
	 * Requires caller init() the LongClockTimer because this method does not.
	 *
	 * maxSaneTimeout is a limit checked on each call to sleepFoo(), for robustness.
	 */
	static void init(
			OSTime maxSaneTimeout,
			LongClockTimer *);

	static void clearReasonForWake();

	/*
	 * Sleep a given time (a func that calculates timeout, or a constant timeout)
	 * Expect only a timer event, but allow for unexpected waking events.
	 */
	static void sleepUntilTimeout(OSTime (*func)());
	static void sleepUntilTimeout(DeltaTime);

	/*
	 * Expect either a timeout or a message event.
	 */
	static HandlingResult sleepUntilMsgAcceptedOrTimeout(
			MessageHandler*,
			OSTime (*func)());

	static msgReceivedCallback getMsgReceivedCallback();
};
