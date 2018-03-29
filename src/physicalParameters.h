/*
 * Measured parameters depend on physics of radio.
 *
 * Durations of real OTA events and other delays
 */

class PhysicalParameters {
public:





/*
 * The delay between the time the sender fetches offset time and sender actually sends it.
 * IOW delay between send and TOA, on a reference clock.
 *
 * This is currently just a guess or measured.
 * If sending code changes, or optimization, this changes too.
 *
 * If senders can vary (mix of nrf51 or nrf52), this varies too.
 *
 * Since it varies by sender, the sender adjusts a sync offset using this.
 *
 * Comprises:
 * - SW overhead
 * - rampup of radio (varies by platform)
 * - OTA time (varies by baud rate)
 *
 * Some components are documented below,
 * but we just measure the total.
 *
 */
#if defined( NRF52 )
	/*
	 * 2 rampup + 2 OTA + ? SW overhead
	 *
	 * SW overhead appears to be about 1 tick
	 *
	 * 4 measured July 2017 no optimization, logging enable, asserts enabled, gcc 6, nrf52DK
	 */
	static const DeltaTime SendLatency = 4;
#elif defined( NRF51)
	/*
	 * 4 rampup + 2 OTA + ? SW overhead
	 *
	 * SW overhead appears to be about 4 ticks, i.e. 4x nrf52
	 *
	 * 10 measured July 2017 no optimization, logging enable, asserts enabled, gcc 6, Waveshare Core
	 */
	static const DeltaTime SendLatency = 10;
#else
#error "NRFxx not defined"
#endif


/*
 * When latency is not in message payload, use this.
 * Currently, most of the units are nRF51
 */
static const DeltaTime GuessedSendLatency = 10;



/*
 * The delay between message received and time TOA was recorded.
 * This is negligible: an interrupt occurs and a few tens of instructions later we record TOA.
 *
 * The time betwen message received and the receiver updates sync (using the offset from the message)
 * is irrelevant since TOA and offset don't change with passing time at receiver.
 *
 * Comprises:
 * SW overhead (to check validity, make calculations, etc.)
 *
 * This too can vary by platform (faster cpu clock.)
 */
//NOT USED static const DeltaTime ReceiveLatency = 0;


/*
 * After system wakes from radio off state,
 * delay until we should activate radio (from DISABLED state to active state).
 * This is one component of 'dead' time for radio.
 * (Even after we activate, their is more dead time: rampup.)
 *
 * Comprises:
 * - time for HFXO clock to stabilize (1200uSec, 40 ticks)
 * - a few ticks for other overhead (some execution time.)
 * - an allowance for variance (expected worst deviation from experiments.)
 *
 * The constant is experimentally measured, for specific HF crystals.
 * If enough allowance is not made,  there will be dead gaps in listening/fishing.
 * This is used to overlap real slots.
 *
 * !!! Should not be more than SlotDuration, else the last FishSlot will lap into SyncSlot
 */
// NOT USED static const DeltaTime PowerOffToActiveDelay = 41;
// TODO check these constants at runtime
#if PowerOffToActiveDelay > VirtualSlotDuration
#error "delay too long"
#endif


/*
 * Duration of message over-the-air (OTA).
 * Units ticks.
 * Function of bitrate, message length, and RTC frequency:
 *
 * OTA ticks = 1/bitrate [second/bits] * messageLength [bits] * RTCFreq [ticks/second]
 *
 * Message bytes 1 preamble 3 address 11 payload 1 CRC => 16 bytes
 * 16 bytes is 128 bits
 * Used in sanity assertions only?
 */
// 1 Mbit bitrate, 128bit message, 32kHz   yields .12mSec = 4 ticks
// 2 Mbit bitrate, 128bit message, 32kHz   yields .064mSec == 64uSec = 2 ticks
// 2 Mbit, 120 bits, 32kHz yields 1.8 ticks

static const DeltaTime MsgOverTheAirTimeInTicks = 2;


/*
 * After radio is enabled, delay until radio is ready for OTA (TXIDLE or RXIDLE).
 * This is another component of 'dead' time.
 * !!! RampupDelay can be incurred again when switching from xmit to rcv or vice versa.
 */
#ifdef NRF52
	// ramp up in fast mode is 40uSec, i.e. 1.3 ticks
	// NOT USED: static const DeltaTime RampupDelay = 2;
#else // NRF51
	// ramp up is 130 uSec i.e. 4.3 ticks
	// NOT USED: static const DeltaTime RampupDelay = 4;
#endif


};
