
#include "intraCliqueManager.h"

#include "scatter.h"

#include "../modules/sendRepeater.h"
#include "../message/message.h"

#include "../syncAgent/syncAgent.h"
#include "../logging/logger.h"
#include <cassert>

namespace {

NetGranularity _granularity;


/*
 * Delayed actions, taken after a Master has informed clique.
 * Action must be delayed, because can't inform clique aftwerward, since clique is changed by these actions.
 */
void onDoneGranularityWithAction() {
	/*
	 * set my own granularity with granularity we remembered.
	 *
	 * Assert self is master.
	 *
	 * After this, clique often disturbed:
	 * hearing more or fewer others.
	 */
	assert(SyncAgent::isSelfMaster());
	Granularity::setGranularity(_granularity);
}

/*
 * No action because we are slave, upstreaming,
 * and we wait to hear from master, downstream.
 */
void onDoneRepeatingUpstream() {
	assert(SyncAgent::isSelfSlave());
	;	// null action
}

void onDoneScatter() {
	assert(SyncAgent::isSelfMaster());
	Scatter::scatter();
}

}


bool IntraCliqueManager::isActive() {
	return SendRepeater::isActive();
}

void IntraCliqueManager::abort() {
	return SendRepeater::stop();
}

void IntraCliqueManager::checkDoneAndEnactControl() {
	SendRepeater::checkDoneAndEnactControl();
}


// XXX for optimality, when slave hears a granularity message, stop repeating to the master.
// another slave may be repeating upstream too?

/*
 * Current design:
 * upstream and downstream messages not distinguished.
 * Both have MasterID of the clique.
 * Master who hears it will start relaying.
 * Slave who hear it will act as if it came from master (even if from slave).
 */


/*
 * We are a slave who heard a provisioning.
 * Relay to master.
 * When done, do not set my own granularity (only when hear message from master or other slave.)
 */
void IntraCliqueManager::doUpstreamCliqueSizeChange(NetGranularity aGranularity) {
	SendRepeater::start(MessageType::ControlNetGranularity,
			static_cast <uint8_t> (aGranularity),
			onDoneRepeatingUpstream);
}

/*
 * We are a master who heard a clique size change relayed by a slave.
 * Relay to all clique members.
 * When done, set my own granularity.
 */
void IntraCliqueManager::doDownstreamCliqueSizeChange(NetGranularity aGranularity) {
	Logger::log("Start downstream granularity");

	// Remember the granularity for later action
	_granularity = aGranularity;

	SendRepeater::start(MessageType::ControlNetGranularity,
			static_cast <uint8_t> (aGranularity),
			onDoneGranularityWithAction);
}

/*
 * Cannot scatter self until done repeating.
 */
void IntraCliqueManager::doUpstreamScatter() {
	Logger::log("Start downstream scatter");

	// 1 is dummy parameter, assert receiver ignores it
	SendRepeater::start(MessageType::ControlScatterClique,
			1,
			onDoneRepeatingUpstream);
}


void IntraCliqueManager::doDownstreamScatter() {
	SendRepeater::start(MessageType::ControlScatterClique,
			1,
			onDoneScatter);
}

MessageType IntraCliqueManager::currentMsgType() { return SendRepeater::currentMsgType(); }
Payload IntraCliqueManager::currentPayload() { return SendRepeater::currentPayload(); }


