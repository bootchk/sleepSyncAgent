#pragma once

#include <services/mailbox.h>



/*
 * Coordinates sending work.
 *
 * Pertinent to work strategy:
 * - "work on demand from other units"
 * - or "work on demand from master"
 * - or even distributed work, where "work" is just maintenance of
 *
 * app mailbox-> WorkOut -> OTA-> other units-> onWorkMsg()
 *
 * The dance is complicated and there may be lags.
 * Work received OTA in last sync period
 * is mailed to app (to do the work)
 * at start of sync period.
 * Work mailed by app is sent OTA this sync slot.
 */

class WorkOut {
public:
	static void init(Mailbox*);

	static bool isNeedSendWork();
	static MailContents fetch();
};
