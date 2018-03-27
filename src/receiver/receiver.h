
#pragma once

#include "../messageHandler/messageHandler.h"


/*
 * startWithHandler() starts a peripheral task on the radio.
 * The peripheral task generates an event on receipt of message.
 * The event's ISR is a Message Handler task (ISR, runs to completion.)
 *
 * MessageHandler does no long-running computation.
 * Sets state of SleepSync algorithm and possibly queues message to app.
 *
 * No other task is usually running (mcu is usually sleeping.)
 * In the current design, all tasks are at the same priority and can not be interrupted by MessageHandler.
 *
 * Another task is always scheduled (task for next slot in schedule.)
 * In other words, there is always a timeout on receiving.
 *
 * The other scheduled task (the timeout task) will turn off receiver.
 * There is a race to turn off receiver.
 * When a message is received just after another scheduled task begins (from timer event)
 * but before the other task can turn off the receiver,
 * the other task will clear the event, and clear the pending interrupt(???)
 *
 * The MessageHandler task MIGHT start the receiver again.
 * If MessageHandler starts receiver again early in its code,
 * it is conceivable that this task may follow itself (interrupt still pending when task completes.)
 */

/*
 * Receiver not distinct from Transmitter.
 * Operations on receiver affect transmitter.
 */
class Receiver {
public:
	static void startWithHandler(MessageHandler);

	/*
	 * Radio stop receiving.
	 * Can be called when not receiving.
	 * If called when radio is transmitting, ends transmission.
	 */
	static void stop();
};
