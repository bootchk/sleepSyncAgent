
#include "sendRepeater.h"

namespace {
bool _isActive;

// remember what we are repeating
MessageType _messageType;
uint8_t _aValue;
}




void SendRepeater::start(MessageType aType, uint8_t value) {
	if (SendRepeater::isActive()) {
		/*
		 * Already repeating a control message.
		 * We received a duplicate, or multiple controllers are provisioning us.
		 */
		// Logger::log("Busy repeating\n");
	}
	else {
		_isActive = true;
		_messageType = aType;
		_aValue = value;
	}
}


MessageType SendRepeater::currentMsgType() { return _messageType; }
WorkPayload SendRepeater::currentPayload(){ return _aValue; }

bool SendRepeater::isActive() { return _isActive; }


bool SendRepeater::shouldSend() {
	if (isActive()) {
		// flip a coin
		// increment and stop self
	}
	else return false;
}


void SendRepeater::stop() {

}
