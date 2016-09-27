#pragma once

#include <inttypes.h>

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */

#include "types.h"	// SystemID


class Radio {
public:
	static const uint8_t MaxMsgLength = 255;

	static void powerOn();
	static void powerOff();
	static bool isPowerOn();


	/*
	 * Enable/disable receipt of messages.
	 * Not about power to the radio.
	 */
	static void startReceiving();
	static void stopReceiving();
	static bool isDisabled();


	/*
	 * Transmit given data.
	 *
	 * Synchronous: blocks until complete.
	 */
	static void transmit(uint8_t data[], uint8_t length);
	/*
	 * Receive into buffer.
	 *
	 * Asynchronous, does not block.
	 */
	static void receive(uint8_t data[], uint8_t length);	// octets

	// TODO unused?
	static void stopXmit();
	static void stopReceive();
};


// Not Radio, but closely associated.
SystemID myID();


