#pragma once

#include <inttypes.h>

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


class Radio {
public:
	static const uint8_t MaxMsgLength = 255;

	static void init(void (*onRcvMsgCallback)());
	static void powerOn();
	static void powerOff();
	static bool isPowerOn();


	/*
	 * Enable/disable receipt of messages.
	 * Not about power to the radio.
	 */
	//static void startReceiving();
	//static void stopReceiving();

	/*
	 * is in idle state
	 * receive completed OR transmit completed
	 * Some platforms? radio enters this state after receiving a message
	 * so there is a race after calling receive() to enter sleeping state expecting receive event.
	 */
	// TODO !isDisabled() equivalent to isReceiving() ?
	static bool isDisabled();


	/*
	 * Transmit given data.
	 *
	 * Synchronous: blocks until complete.
	 */
	static void transmit(volatile uint8_t data[], uint8_t length);
	/*
	 * Receive into buffer.
	 *
	 * Asynchronous, does not block.
	 */
	static void receive(volatile uint8_t data[], uint8_t length);	// octets

	// TODO unused?
	static void stopXmit();
	static void stopReceive();
};





