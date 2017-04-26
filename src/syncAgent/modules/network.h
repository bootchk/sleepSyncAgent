#pragma once

/*
 * Layer atop radio.
 *
 * Using radio requires an assemble of devices:
 * - radio
 * - HFXTAL oscillator
 *
 * This understands that.  Many methods just do something with both devices.
 *
 * Note the chip powers down radio automatically when not in use,
 * and radio configuration persists through that.
 *
 * FUTURE make network own radio.  Currently, other code calls radio methods.
 */

class Network {
public:
	// In order generally called

	// Ensure both devices ready
	static void startup();
	// Ensure both devices low power
	static void shutdown();

	//static void prepareToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();


	// Is radio in use?
	// If false, radio may be low power but HFXO may still be on
	static bool isRadioInUse();

	// Are all devices in ensemble in low power state?
	static bool isLowPower();

	static bool isConfigured();

};
