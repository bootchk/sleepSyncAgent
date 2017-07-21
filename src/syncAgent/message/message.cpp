
#include "message.h"
#include "../scheduleParameters.h"



LongTime SyncMessage::timeOfArrival;


// TODO only the sender knows sendlatency
LongTime SyncMessage::timeOfTransmittal() {
	return timeOfArrival - PhysicalParameters::SendLatency;
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

