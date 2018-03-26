
#include "fishSlot.h"

#include "../../messageHandler/messageHandler.h"
#include "../../radio/radio.h"


void FishSlot::beginListen() {
	Radio2::startReceivingWithHandler(FishSlotMessageHandler::handle);
}


void FishSlot::dispatchFishSlotKind() {

}
