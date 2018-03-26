
#include "fishSlot.h"

#include "../../messageHandler/messageHandler.h"
#include "../../receiver/receiver.h"


void FishSlot::beginListen() {
	// Handler handles all fishing modes (Trolling and Deep)
	Receiver::startWithHandler(FishSlotMessageHandler::handle);
}
