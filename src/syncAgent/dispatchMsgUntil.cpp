/*
 *
 */
#include "syncAgent.h"

/*
 * Sleep.
 * Wake on msg received or timeout.
 * Dispatch any messages.
 * When dispatcher returns true, sleep until timeout.
 */
void SyncAgent::dispatchMsgUntil() {
	sleepUntilMsgOrTimeout();
}
