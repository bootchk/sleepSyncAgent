
#include "message.h"
#include "../scheduleParameters.h"



/*
 * Only the sender knows sendlatency (varies by platform.)
 * Thus this is an approximation, using this units SendLatency.
 */

// XXX send the latency in the Work byte, ins some messages?

LongTime SyncMessage::timeOfTransmittal() {
	return Radio::timeOfArrival() - PhysicalParameters::GuessedSendLatency;
}




char const * SyncMessage::representation(SyncMessage* msg) {
	char const * result;

	switch(msg->type) {
		case MessageType::MasterSync:
			result = "MastS";
			break;
		case MessageType::MasterMergedAway:
			result = "MSMAw";
			break;
		case MessageType::SlaveMergedAway:
			result = "MSSAw";
			break;
		case MessageType::EnticingInferior:
			result = "MSEnt";
			break;
		case MessageType::WorkSync:
			result = "WorkS";
			break;

		case MessageType::AbandonMastership:
			result = " Aban";
			break;
		case MessageType::Info:
			result = " Info";
			break;
		case MessageType::WorkSetProximity:
			result = " Prox";
			break;
		case MessageType::WorkScatterTime:
			result = " WSca";
			break;
		case MessageType::ControlNetGranularity:
			result = " CGra";
			break;
		case MessageType::ControlScatterClique:
			result = " CSca";
			break;
	}
	return result;
}

