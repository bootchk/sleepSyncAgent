
#pragma once

/*
 * Knows actions at end of deep fishing.
 */
class DeepFishing {
public:
	/*
	 * Callbacks registered with FishingManager
	 */
	static void endWithNoAction();
	static void endWithRecoverMaster();
};
