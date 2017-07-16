
#include "message.h"
#include "../scheduleParameters.h"



LongTime SyncMessage::timeOfArrival;

LongTime SyncMessage::timeOfTransmittal() {
	return timeOfArrival - ScheduleParameters::SenderLatency;
}

