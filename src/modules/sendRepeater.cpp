
#include "sendRepeater.h"

#include <random.h>	// randBool
#include <cassert>


namespace {
bool _isActive;

// remember what we are repeating
MessageType _messageType;
uint8_t _aValue;

// callback when done
Callback _onDoneCallback;

unsigned int counter;
}




void SendRepeater::start(
		MessageType aType,
		uint8_t value,
		Callback onDoneCallback)
{
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
		_onDoneCallback = onDoneCallback;

		counter = 0;
	}
}


MessageType SendRepeater::currentMsgType() { return _messageType; }
WorkPayload SendRepeater::currentPayload(){ return _aValue; }

bool SendRepeater::isActive() { return _isActive; }


bool SendRepeater::shouldSend() {
	bool result = false;

	// If previous call was the last


	if (isActive()) {
		if (randBool()) { // flip a coin
			result = true;
			counter++;
		}
	}
	return result;
}

// TODO parameter
void SendRepeater::checkDoneAndEnactControl() {
	// require only call when has been started and shouldSend returned true
	// sanity on state
	assert(_isActive);

	if (counter > 4 ) {
		_isActive = false;

		_onDoneCallback();
	}
}


void SendRepeater::stop() {

}
