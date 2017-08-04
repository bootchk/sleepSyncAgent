#pragma once

/*
 * Introduces random delay waiting until start fishing sequence.
 * Avoid all members of clique fishing in same slot when slots are fished in a known sequence.
 */
class FishWaitingPolicy {
public:

	static void startFishWaiting();

	/*
	 * Advance the wait and possible change to Role::Fisher
	 */
	static void advance();
};
