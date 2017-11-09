
#pragma once



class FishSlot{
private:
	static void perform();

public:
	// perform if power
	static void tryPerform();

	/*
	 * Callbacks registered with FishingManager
	 */
	static void endDeepFishingWithNoAction();
	static void endDeepFishingWithRecoverMaster();
};
