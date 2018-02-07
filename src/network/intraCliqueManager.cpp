
#include "intraCliqueManager.h"

#include "../modules/sendRepeater.h"
#include "../message/message.h"


/*
 * Upstream and downstream repeat to ensure delivery.
 *
 *
 */


bool IntraCliqueManager::isNeedSend() {
	return SendRepeater::isActive();
}


/*
 * Current design:
 * upstream and downstream messages not distinguished.
 * Both have MasterID of the clique.
 * Master who hears it will start relaying.
 * Slave who hear it will act as if it came from slave.
 */
/*
 * We are a slave who heard a provisioning.
 * Relay to master.
 */
void IntraCliqueManager::doUpstreamCliqueSizeChange(NetGranularity aGranularity) {
	SendRepeater::start(MessageType::ControlNetGranularity,  static_cast <uint8_t> (aGranularity) );
}

/*
 * We are a master who heard a clique size change relayed by a slave.
 * Relay to all clique members.
 */
void IntraCliqueManager::doDownstreamCliqueSizeChange(NetGranularity aGranularity) {
	SendRepeater::start(MessageType::ControlNetGranularity,  static_cast <uint8_t> (aGranularity) );
}


void IntraCliqueManager::doUpstreamScatter() {
	// 1 is dummy parameter, assert receiver ignores it
	SendRepeater::start(MessageType::ControlScatterClique,  1 );
}


void IntraCliqueManager::doDownstreamScatter() {
	SendRepeater::start(MessageType::ControlScatterClique,  1 );
}

MessageType IntraCliqueManager::currentMsgType() { return SendRepeater::currentMsgType(); }
WorkPayload IntraCliqueManager::currentPayload() { return SendRepeater::currentPayload(); }
