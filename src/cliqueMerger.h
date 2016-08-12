
#include <cassert>

/*
 * Record of a Clique merging into a better Clique.
 *
 * Invariant: Role.isMerger => CliqueMerger.isActive()
 *
 * Singleton.
 */
class CliqueMerger {

public:
	// Compiler defaults ctor
	//SyncAgent();
	//static Schedule schedule;

	void adjustBySyncMsg(Message msg) {
		// TODO
		;
	}

	bool isActive() {
		return active;
	}

	bool shouldScheduleMerge() {
		assert(isActive());	// require
		// TODO CA, random chance
		return true;
	}
	// called after sending a merging sync
	bool checkCompletionOfMergerRole() {
		assert(isActive());	// require
		notifyCountdown--;
		bool result = false;
		if ( notifyCountdown <= 0 ){
			result = true;
			active = false;
		}
		return result;
	}

private:
	static bool active;
	static int offsetToMergee;
	static int masterID;
	static int notifyCountdown;
};
