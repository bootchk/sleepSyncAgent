
/*
 * Example main for app using SleepSyncAgent.
 *
 * Deal with RTOS:
 * - tasks
 * - queues
 * - standard startup boilerplate
 *
 * RTOS objects:
 *
 * Tasks/Threads, in order of priority
 * - hwi/ISR for timer for work (FUTURE)
 * - radioTask (stack provided by platform)
 * - syncTask (provided here)
 * - workTask
 *
 * Queues:
 * - msg queue from radioStack to syncTask
 * - msg queue from syncTask to radioStack
 * - msg queue from workTask to syncTask
 * - msg queue from syncTask to workTask
 *
 * Semaphores
 * - misc (provided by platform for msg queues, hidden e.g. by ICall)
 *
 * Priority
 * Here I assume work is lower priority than sync.
 * Sync is higher priority because work is not hard-realtime while sync messages are timing critical.
 * Power and priority are intertwined. I assume that work takes more power than sync.
 * Attempt to maintain sync even if there is not enough power to do work.
 * You can change that, you could even implement hysteresis:
 * important to achieve sync while power is rising,
 * but important to do work while power is falling but there is sync.
 *
 * Copyright 2016 lloyd konneker
 */



#include "sleepSyncAgent.h"



void onWorkMsgQueued() {
	// SleepSyncAgent received and queued a work msg.
	// TODO schedule low priority work thread/task to do work
	// realtime constrained
}


SleepSyncAgent sleepSyncAgent(onWorkMsgQueued);


int main() {
	// assert embedded system startup is done and calls main.
	sleepSyncAgent.loopOnEvents();	// never returns
	return 0;
}
