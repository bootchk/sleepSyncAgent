
#include "fishSlot.h"

#include "../../messageHandler/messageHandler.h"
#include "../../receiver/receiver.h"


void FishSlot::beginListen() {
	Receiver::startWithHandler(FishSlotMessageHandler::handle);
}


void FishSlot::dispatchFishSlotKind() {

}
