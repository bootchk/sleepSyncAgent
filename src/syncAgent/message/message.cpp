
#include "message.h"
#include "../scheduleParameters.h"



LongTime SyncMessage::timeOfArrival;


/*
 * Only the sender knows sendlatency (varies by platform.)
 * Thus this is an approximation, using this units SendLatency.
 */
// FIXME
/*
 * We could send the latency in the Work byte, ins some messages?
 */
LongTime SyncMessage::timeOfTransmittal() {
	return timeOfArrival - PhysicalParameters::GuessedSendLatency;
}




char const * SyncMessage::representation(SyncMessage* msg) {
	char const * result;

	switch(msg->type) {
		case MessageType::MasterSync:
			result = "MastS";
			break;
		case MessageType::MergeSync:
			result = "MergS";
			break;
		case MessageType::WorkSync:
			result = "WorkS";
			break;

		case MessageType::AbandonMastership:
			result = " Aban";
			break;
	}
	return result;
}

