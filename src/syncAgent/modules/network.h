#pragma once

/*
 * Layer atop radio.
 */

class Network {
public:
	// In order generally called

	// Preamble mainly starts HfCrystalClock needed by radio
	static void preamble();

	static void prepareToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();
	static void shutdown();
	static void postlude();

};
