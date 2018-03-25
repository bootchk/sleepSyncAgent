
#include "syncSlotProperty.h"

#include "../../modules/syncBehaviour.h"
#include "../../network/intraCliqueManager.h"
#include "../../work/workOut.h"



/*
 * order prioritizes
 * Also must call shouldTransmitSync and shouldSendControlSync to count down
 *
 * Work is higher priority than ordinary sync.
 * Work must be rare, lest it flood airwaves and destroy sync.
 * (colliding too often with MergeSync or MasterSync.)
 *
 * App (user of SyncAgent) decides policy and conveys  (queues) work to be sent.
 */
SyncSlotKind SyncSlotProperty::decideKind() {
	SyncSlotKind result;

	/*
	 * Call shouldTransmitSync each sync slot: it ticks an alarming clock.
	 * Result is true if alarm, meaning: need xmit sync to prevent long gaps in sync transmittals.
	 */
	bool needXmitSync = SyncBehaviour::shouldTransmitSync();

	if (IntraCliqueManager::IntraCliqueManager::shouldSendControlSync()) result = SyncSlotKind::sendControlSync;
	else if (WorkOut::isNeedSendWork())                                  result = SyncSlotKind::sendWorkSync;
	else if (needXmitSync)                                               result = SyncSlotKind::sendSync;
	else                                                                 result = SyncSlotKind::listen;
		// Listen means isSlave OR (isMaster and not sending sync (coin flip))

	return result;
}


