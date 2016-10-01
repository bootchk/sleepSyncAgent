#pragma once

#include <inttypes.h>

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


class Radio {
public:
	static const uint8_t MaxMsgLength = 255;

	static void init(void (*onRcvMsgCallback)());
	static void powerOnAndConfigure();
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
	// FUTURE !isDisabled() equivalent to isReceiving() ?
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
	/*
	 * Transmit one message and wait til it is transmitted.
	 */
	static void transmitStaticSynchronously();
	/*
	 * Configure radio to receive one message.
	 * Message might NOT be received (if no transmitters, or garbled receipt.)
	 * If message is received, waked from sleep and reasonForWakeIsMsg() returns true, etc.
	 * TODO what does etc. mean?
	 */
	static void receiveStatic();
	/*
	 * Undo the effect of receiveStatic.
	 * This includes undoing enabling of any interrupts that wake on receive.
	 * This does not include clearing any buffers from received messages
	 * that might be in a race?
	 */
	static void stopReceive();

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
	// FUTURE we could use isReadyToReceive() to assert (instead of !isDisabled() )
};





