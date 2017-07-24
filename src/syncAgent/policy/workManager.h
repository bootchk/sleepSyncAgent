#pragma once

#include <services/mailbox.h>



/*
 * Coordinates sending and receiving of work.
 *
 * Work is done at start of sync period based on work heard in last sync period.
 *
 */

class WorkManager {
public:
	static void init(Mailbox*);

	static bool isNeedSendWork();
	static WorkPayload fetch();

	static void resetState();
	static void hearWork();
	static bool isHeardWork();
};

