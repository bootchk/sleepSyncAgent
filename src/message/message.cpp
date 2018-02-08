
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
		case MessageType::WorkSync:
			result = "WorkS";
			break;
		case MessageType::ControlNetGranularity:
			result = " CGra";
			break;
		case MessageType::ControlScatterClique:
			result = " CSca";
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

		case MessageType::AbandonMastership:
			result = " Aban";
			break;
		case MessageType::Info:
			result = " Info";
			break;
		/* case MessageType::WorkSetProximity:
			result = " Prox";
			break;
		case MessageType::WorkScatterTime:
			result = " WSca";
			break;
		*/

		case MessageType::Invalid:
			result = " Inva";
			break;
	}
	return result;
}


// Does an OTA received byte seem like a MessageType?
MessageType SyncMessage::messageTypeFromRaw(uint8_t receivedType) {
	MessageType result;

	switch(receivedType) {
	case 1:  result = MessageType::MasterSync; break;
	case 2:  result = MessageType::WorkSync; break;
	case 3:  result = MessageType::ControlNetGranularity; break;
	case 4:  result = MessageType::ControlScatterClique; break;

	case 5:  result = MessageType::EnticingInferior; break;
	case 6:  result = MessageType::MasterMergedAway; break;
	case 7:  result = MessageType::SlaveMergedAway; break;

	case 8:  result = MessageType::AbandonMastership; break;
	case 9:  result = MessageType::Info; break;

	default: result = MessageType::Invalid; break;
	}
	return result;
}




bool SyncMessage::doesCarrySync(MessageType type) {
	bool result;
	switch (type){
	case MessageType::MasterSync:
	case MessageType::WorkSync:
	case MessageType::ControlNetGranularity:
	case MessageType::ControlScatterClique:
		result = true;
		break;

	default:
		result = false;
	}
	return result;
}


#ifdef NOT_USED
/*
 * Is message related to merging?
 */
// NOT USED?
static bool doesCarryOffsetToOtherClique(MessageType type) {
	bool result;
	switch (type){
	case MessageType::MasterSync:
	case MessageType::EnticingInferior:
	case MessageType::WorkSync:
		result = true;
		break;
	default:
		result = false;
	}
	return result;
}
#endif
