#pragma once

#include <inttypes.h>

/*
 * Wrapper aka abstraction layer for software stack for radio/wireless
 */


class Radio {
public:
	/*
	 * Length of payload buffer owned by device.
	 */

	static const uint8_t FixedPayloadCount = 10;
	// FUTURE, when messages are DYNAMIC (variable-length) static const uint8_t MaxMsgLength = 255;

	static void init(void (*onRcvMsgCallback)());
	static void powerOnAndConfigure();
	static void configureXmitPower(unsigned int dBm);
	static void powerOff();
	static bool isPowerOn();

	/*
	 * is in idle state?
	 * receive completed OR transmit completed OR just powered on
	 * Some platforms? radio enters this state after receiving a message
	 * so there is a race after calling receive() to enter sleeping state expecting receive event.
	 */
	// FUTURE !isDisabledState() equivalent to isReceiving() ?
	static bool isDisabledState();

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
	 * Post condition:
	 *  isEnabledInterruptForMsgReceived()
	 *  AND ! isDisabled()
	 *
	 * Message might NOT be received (if no transmitters, or garbled receipt.)
	 * If message is received:
	 * - waked from sleep and reasonForWakeIsMsg() returns true
	 * - radio's buffer ( at getBufferAddress) is changed
	 */
	static void receiveStatic();

	/*
	 * Is interrupt enabled on msg received event?
	 */
	static bool isEnabledInterruptForMsgReceived();


	/*
	 * Undo the effect of receiveStatic.
	 * This includes undoing enabling of any interrupts that wake on receive.
	 * This does not include clearing any buffers from received messages.
	 * This does not guarantee that any queue of received messages is empty,
	 * since a message can be received just before the call to this
	 * (a race after checking queue to stopReceive.)
	 */
	static void stopReceive();

	/*
	 * Get address of single buffer used by radio for single-buffered rcv and xmit.
	 */
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

	static bool isPacketCRCValid();
};





