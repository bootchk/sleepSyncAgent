#pragma once


/*
 * Coordinates sending and receiving of work.
 *
 * Work is done at start of sync period based on work heard in last sync period.
 *
 */

class WorkManager {
public:
	static void resetState();
	static void hearWork();
	static bool isNeedSendWork();
	static bool isHeardWork();
};

