#pragma once


#include "../message/message.h"

/*
 * Knows how to fabricate and transmit/broadcast a flavor of Sync msg.
 *
 * Collaborates with Radio and Serializer.
 *
 * Note that the caller checked that there is enough power for radio.
 * The power MIGHT have fallen since then, but it is unlikely
 * since only a little receiving has been done.
 * FUTURE: check power more often and abandon in the middle of a slot
 * or in the middle of a sync period (omit listening in the fishSlot)?
 */


class SyncSender {
public:

	/*
	 * Make MasterSyncMessage, having:
	 * - type MasterSync
	 * - self ID
	 * - forwardOffset unsigned delta now to next SyncPoint
	 * - work = net granularity !!!
	 */
	static void sendMasterSync();

	static void sendWorkSync(Payload work);
	static void sendMergeSync();
	static void sendAbandonMastership();
	static void sendInfo(Payload work);

	static void sendControlSync();
};
