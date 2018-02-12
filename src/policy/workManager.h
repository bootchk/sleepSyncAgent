#pragma once

#include <services/mailbox.h>



/*
 * Coordinates sending and receiving of work.
 *
 * Pertinent to work strategy: "work on demand from other units" or "work on demand from master"
 *
 * app <-mailbox-> WorkManager <-OTA-> other units
 *
 * The dance is complicated and there may be lags.
 * Work received OTA in last sync period
 * is mailed to app (to do the work)
 * at start of sync period.
 * Work mailed by app is sent OTA this sync slot.
 */

class WorkManager {
public:
	static void init(Mailbox*);

	static bool isNeedSendWork();
	static MailContents fetch();

	static void resetState();
	static void hearWork();
	static bool isHeardWork();
};

