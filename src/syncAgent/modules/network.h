#pragma once

/*
 * Layer atop radio.
 *
 * Using radio requires an assemble of devices:
 * - radio
 * - HFXTAL oscillator
 *
 * This coordinates.
 *
 * Note the chip powers down radio automatically when not in use,
 * and radio configuration persists through that.
 */

class Network {
public:
	// In order generally called

	// Preamble mainly starts HfCrystalClock needed by radio
	static void preamble();

	static void prepareToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();
	//static void shutdown();
	static void postlude();


	// Is radio in use?
	// If false, radio may be low power but HFXO may still be on
	static bool isRadioInUse();

	// Are all devices in ensemble in low power state?
	static bool isLowPower();

	static bool isConfigured();

};
