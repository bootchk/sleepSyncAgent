
#include "message.h"
#include "../scheduleParameters.h"



LongTime SyncMessage::timeOfArrival;

LongTime SyncMessage::timeOfTransmittal() {
	return timeOfArrival - ScheduleParameters::SenderLatency;
}

char const * SyncMessage::representation(SyncMessage* msg) {
	char * result;

	switch(msg->type) {
		case MessageType::MasterSync:
			result = "MSyn";
			break;
		case MessageType::MergeSync:
			result = "Merg";
			break;
		case MessageType::AbandonMastership:
			result = "Aban";
			break;
		case MessageType::WorkSync:
			result = "Work";
	}
	return result;
}

