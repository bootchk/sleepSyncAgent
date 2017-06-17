#pragma once

/*
 * Layer atop radio.
 *
 * Using radio requires an ensemble of devices:
 * - radio
 * - HFXO xtal oscillator
 * - DCDC power supply (optional for power efficiency of about 20%)
 *
 * This understands that.  Many methods just do something with ensemble devices.
 *
 * Note the chip powers down radio automatically when not in use,
 * and radio configuration persists through that.
 *
 * Algebra of calls:
 * Typical:
 * config(); startup(); startReceiving(); (received) ...startReceiving(); (notReceive); stopReceiving(); shutdown(); startup()
 *
 * config() need only be called once, the configuration persists.
 *
 * startReceiving() can be called repeatedly.
 * It SHOULD be called after a msg is received (even if message is invalid)
 *
 * stopReceiving() should be called if no msg is received and you want to disable reception.
 *
 * There is a race after startReceiving(): isInUse() may return false as soon as a msg is received
 *
 * stopReceiving(); assert(! isInUse()); is valid.
 *
 * startReceiving(); (msgReceived); assert(!isInUse()); is valid: a msg received means radio disabled from receiving again
 * until you startReceving() again.
 */

// XXX make network own radio.  Currently, other code calls radio methods.

class Network {
public:
	// In order generally called

	// Ensure ensemble devices ready
	static void startup();

	// Ensure ensemble devices low power
	static void shutdown();

	// Non-blocking, but lag (deadtime) for rampup until can hear
	static void startReceiving();

	static void stopReceiving();

	// Blocks.  Lag before OTA.
	static void transmitStaticSynchronously();

	/*
	 * Illegal to call when network is shutdown (power off.)
	 * If false, radio may be low power but HFXO may still be on
	 */
	static bool isRadioInUse();

	// Are ensemble devices in low power state?
	static bool isLowPower();

	static bool isConfigured();
};
