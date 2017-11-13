#pragma once

#include <inttypes.h>


/*
 * A slot similar to a SyncSlot (replaces it.)
 * I.e. in sync with other clique members.
 *
 * Sends msg of type Info.
 * Has no meaning to sync algorithm.
 *
 * Info msg should be rare, since it contends with Sync messages
 * and can disturb sync.
 *
 * Useful for debugging with the DK, since the DK (if a member)
 * can hear other members and log their info messages.
 * (Sniffer will also log Info messages, from any clique.)
 */

class InfoSlot{

public:
	/*
	 * perform regardless of power
	 *
	 * System might be foobar: brownout, assertion, etc.
	 */
	static void perform(uint8_t);
};
