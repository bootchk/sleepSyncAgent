
#pragma once

/*
 * FishingMode is (roughly speaking) a subclass of MergerFisherRole::Fisher.
 * Each FishingMode is associated with a FishingPolicy
 *
 * Trolling is non-episodic: continues indefinitely, wrapping around.
 * DeepFishing is episodic: finite, and generates an event on completion.
 *
 * Caller knows when fishing succeeds (fish another clique's Master.)
 * Generally that entails a switch to another FishingMode.
 *
 * When DeepFishing, another request to switchDeepFishing() might be equivalent,
 * and does not restart DeepFishing.
 * Otherwise, it restarts DeepFishing(internally)
 *
 */
enum class FishingMode {
	Trolling,	// Scanning across sync period, by slots.
	DeepFishing	// Repetitively fishing one sub-period of sync period (not necessarily a slot.)
};
