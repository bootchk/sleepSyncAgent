
#pragma once

#include "../messageHandler/messageHandler.h"


/*
 * Task spawned on receipt of message by radio peripheral.
 *
 * Runs as ISR, to completion.
 *
 * No long-running computation is done.
 * Sets state of SleepSync algorithm and possibly queues message to app.
 *
 * No other task is usually running (mcu is usually sleeping.)
 * In the current design, all tasks are at the same priority and can not be interrupted by this task.
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
 * This task MIGHT start the receiver again.
 * If this task starts receiver again early in the task,
 * it is conceivable that this task may follow itself (interrupt still pending when task completes.)
 */
class Receiver {
public:
	static void startWithHandler(MessageHandler);
};
