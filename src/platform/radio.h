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

#ifdef DYNAMIC
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
#endif
	static void transmitStaticSynchronously();
	static void receiveStatic();
	//FUTURE static void getBufferAddressAndLength(uint8_t** handle, uint8_t* lengthPtr);
	static uint8_t* getBufferAddress();

	/*
	 * FUTURE semantics: cancel radio operation or change mode without powering off
	 * For now, we don't need since SyncAgent:
	 * - xmits first, leaving radio ready to receive
	 * - after msg received, radio ready to receive again
	 * - uses powerOff() to end receive mode
	 *
	 * static void stopXmit();
	 * static void stopReceive();
	 */
	// TODO we could use isReadyToReceive() to assert (instead of !isDisabled() )
};





